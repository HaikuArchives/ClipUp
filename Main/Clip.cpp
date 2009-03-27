#include "Clip.h"

#include <Bitmap.h>
#include <Clipboard.h>
#include <Font.h>
#include <Mime.h>
#include <String.h>

#include <string.h>

#include "DeskbarView.h"
#include "get_resource_bitmap.h"

Clip::Clip( const BMessage *msg,
			const char *application_signature,
			bool is_protected )
:	fSignature( application_signature ),
	fIsProtected( is_protected ),
	fFlatData( NULL )

{
	
	if (msg) fClip = *msg;
	
	CompleteInit();
}

Clip::~Clip()
{
	delete fFlatData;
}

// belegt Felder wie Icon etc. vor, nachdem die Hauptfelder initialisiert wurden
void Clip::CompleteInit()
{
	
	if (fClip.IsEmpty()) {
		delete fFlatData;
		fFlatData = NULL;

		fBubbleText = "";
		return;
	}
	
	// set flat data
	if (fFlatData) delete fFlatData;
	
	ssize_t size = fClip.FlattenedSize();
	fFlatData = new char [size];
	fClip.Flatten(fFlatData, size);
	
}

bool Clip::IsEmpty() const
{
	return (fClip.IsEmpty());
}

bool Clip::IsProtected() const
{
	return (fIsProtected);
}

bool Clip::IsFixedSize() const
{
	return false;
}

type_code Clip::TypeCode() const
{
	return B_RAW_TYPE;
}

ssize_t Clip::FlattenedSize() const
{
	return	4
		+ 4 + sizeof(int32) + fClip.FlattenedSize()
		+ 4 + sizeof(int32) + fSignature.length()
		+ 4 + sizeof(int32) + sizeof(fIsProtected);

}

status_t Clip::Flatten(void *buffer, ssize_t buf_size) const
{

	if (buf_size < FlattenedSize()) return B_NO_MEMORY;

	if (fClip.IsEmpty()) return B_OK;
	
	int32	size;
	
	// id
	string	data = "clip";
	
	// data
	data += "data";
	size = fClip.FlattenedSize();
	data.append((char *)&size, sizeof(int32));
	data.append(fFlatData, size );
	
	// app_signature
	data += "sign";
	size = fSignature.length();
	data.append((char *)&size, sizeof(int32));
	data.append(fSignature.data(), size);
	
	// protected
	data += "prot";
	size = sizeof(bool);
	data.append((char *)&size, sizeof(int32));
	data.append((char *)&fIsProtected, size);
	
	memcpy(buffer, data.data(), data.length());
	
	return B_OK;
}

status_t Clip::Unflatten(type_code c, const void *buf, ssize_t size)
{
	if (c!=B_RAW_TYPE) return B_BAD_TYPE;
	
	char *data = (char *)buf;
	if (strncmp("clip", data, 4)!=0) return B_ERROR;
	
	data += 4;
	
	while ((long)buf + size > (long)data + 8) {
		
		string	code;
		code.assign( data, 4);
		
		int		code_size = *(int *)(data + 4);
		
		data += 8;
		
		if ((long)data + code_size > (long)buf + size) return B_ERROR;
		
		if (size!=0) {
			if (code=="data") {
				if (fClip.Unflatten(data)!=B_OK) return B_ERROR;
			}
			else if (code=="sign") {
				fSignature.assign(data, code_size);
			}
			else if (code=="prot") {
				memcpy (&fIsProtected, data, code_size);
			}
		}

		data += code_size;
	}

	CompleteInit();

	return B_OK;
}

const char *Clip::BubbleText()
{
	if (!fBubbleText.empty()) return fBubbleText.data();
	
	BString	title;
	
	// Ist es eine Klasse?
	if ( fClip.what == B_ARCHIVED_OBJECT && fClip.FindString("class", &title)==B_OK) {
		fBubbleText = title.String();
		fBubbleText += char(0);
		return fBubbleText.data();
	}
	
	// ansonsten alle MIMEs rausholen...
	char		*nameFound;
	type_code	typeFound;

	BString		plain_text;
	BMimeType	first_mime;
	bool		found_first_mime = false;
	
	for (int i=0; ; ++i) {
	
		// Wir gehen davon aus, dass der Programmierer sich für nur einen der Typen entscheidet!
		int32 	countFound;
		uint	types[] = { B_MIME_TYPE, B_MESSAGE_TYPE, B_STRING_TYPE };
		for (int j=0; j<(int)(sizeof(types)/sizeof(uint32)); ++j) {
			if ( (fClip.GetInfo(types[j], i, &nameFound, &typeFound, &countFound) == B_OK) )
				break;
		}
			
		if (!countFound) break;
		
		BMimeType	mime( nameFound );
		if (!mime.IsValid()) break;
		
		if (!found_first_mime) {
			first_mime.SetTo(mime.Type());
			found_first_mime = true;
		}
		
		if (mime=="text/plain") {

			const char	*data;
			ssize_t	numBytes;
			
			fClip.FindData(nameFound, typeFound, i, (const void **)&data, &numBytes );
			
			title.Append(data, numBytes);
			
			int32	pos = 0;
			BFont	font( be_plain_font );
			int32	lines = 0;
			int32	length = title.Length();
			
			bool last_loop = false;
			
			do {
				int32 nextpos = title.FindFirst( "\n", pos );

				if (nextpos==B_ERROR) {
					last_loop = true;
					nextpos = length;
				}
		
				BString	temp;
				title.CopyInto(temp, pos, nextpos - pos);
				temp.ReplaceAll("\t", "   ");
				font.TruncateString(&temp, B_TRUNCATE_END, 300.0 );
				fBubbleText += temp.String();
				if (!last_loop) fBubbleText += "\n";

				++lines;
				if (lines>30 && !last_loop) {
					fBubbleText += "\n\t...";
					break;
				}
				
				pos = nextpos + 1;
				
			} while (!last_loop);
			
			fBubbleText += char(0);
			return fBubbleText.data();

		}
	}

	// wenn wir keinen Text haben, aber einen anderen MimeType
	if (found_first_mime) {
		char description[B_MIME_TYPE_LENGTH];
		fBubbleText = "<";
		if (first_mime.GetShortDescription(description)==B_OK)
			fBubbleText += description;
		else
			fBubbleText += first_mime.Type();
		fBubbleText += ">";
	}
	else {	// und wenn gar nichts gefunden wird, auf "unknown clip" setzen
		fBubbleText = "<Unknown clip>";
	}
	fBubbleText += char(0);
	return fBubbleText.data();
}

bool Clip::operator== (const Clip& rhs)
{
	if (fClip.FlattenedSize()!=rhs.fClip.FlattenedSize()) return false;
	
	return memcmp(fFlatData, rhs.fFlatData, fClip.FlattenedSize())==0;
}


void Clip::WriteToClipboard()
{
	if (fClip.IsEmpty()) return;
	
	be_clipboard->StopWatching(BMessenger(gDeskbarView));

	if (be_clipboard->Lock()) {

		be_clipboard->Clear();

		BMessage *clip = be_clipboard->Data();

		if (clip) {
			*clip = fClip;
			be_clipboard->Commit();
		}
	
		be_clipboard->Unlock();
	}

	be_clipboard->StartWatching(BMessenger(gDeskbarView));

}


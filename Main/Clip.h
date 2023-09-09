/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _CLIP_H
#define _CLIP_H

#include <Flattenable.h>
#include <Message.h>

#include <string>

class Clip : public BFlattenable {

friend class ClipView;

public:
				Clip(	const BMessage *msg = NULL,
#if __INTEL__
						const char *application_signature = "application/x-vnd.Be-elfexecutable",
#else /* PPC */
						const char *application_signature = "application/x-vnd.Be-executable",
#endif
						bool is_protected = false );
					
virtual			~Clip();

void			CompleteInit();

const char		*BubbleText();

bool			IsEmpty() const;
bool			IsProtected() const;

void			WriteToClipboard();

// implement BFlattenable
bool			IsFixedSize() const;
type_code		TypeCode() const;
ssize_t			FlattenedSize() const;
status_t		Flatten(void *buffer, ssize_t size) const;
status_t		Unflatten(type_code c, const void *buf, ssize_t size);

bool			operator== (const Clip& rhs);

private:

BMessage		fClip;							// the original clip
std::string		fSignature;						// application which created the clip
std::string		fBubbleText;					// text for bubble help
bool			fIsProtected;					// clip is protected

char			*fFlatData;						// flattened original clip

};

#endif

/*
 * Copyright (C) 2012-2013 Robin Haberkorn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>

#include <Scintilla.h>

#include "sciteco.h"
#include "interface.h"
#include "undo.h"
#include "document.h"

void
TECODocument::edit(void)
{
	if (!is_initialized())
		doc = (SciDoc)interface.ssm(SCI_CREATEDOCUMENT);

	interface.ssm(SCI_SETDOCPOINTER, 0, (sptr_t)doc);
	interface.ssm(SCI_SETFIRSTVISIBLELINE, first_line);
	interface.ssm(SCI_SETXOFFSET, xoffset);
	interface.ssm(SCI_SETSEL, anchor, (sptr_t)dot);
}

void
TECODocument::undo_edit(void)
{
	undo.push_msg(SCI_SETSEL, anchor, (sptr_t)dot);
	undo.push_msg(SCI_SETXOFFSET, xoffset);
	undo.push_msg(SCI_SETFIRSTVISIBLELINE, first_line);
	undo.push_msg(SCI_SETDOCPOINTER, 0, (sptr_t)doc);
}

void
TECODocument::update(void)
{
	anchor = interface.ssm(SCI_GETANCHOR);
	dot = interface.ssm(SCI_GETCURRENTPOS);
	first_line = interface.ssm(SCI_GETFIRSTVISIBLELINE);
	xoffset = interface.ssm(SCI_GETXOFFSET);
}

/*
 * Only for QRegisterStack::pop() which does some clever
 * exchanging of document data (without any deep copying)
 */
void
TECODocument::exchange(TECODocument &other)
{
	SciDoc temp_doc = doc;
	gint temp_anchor = anchor;
	gint temp_dot = dot;
	gint temp_first_line = first_line;
	gint temp_xoffset = xoffset;

	doc = other.doc;
	anchor = other.anchor;
	dot = other.dot;
	first_line = other.first_line;
	xoffset = other.xoffset;

	other.doc = temp_doc;
	other.anchor = temp_anchor;
	other.dot = temp_dot;
	other.first_line = temp_first_line;
	other.xoffset = temp_xoffset;
}

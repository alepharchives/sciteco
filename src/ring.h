#ifndef __RING_H
#define __RING_H

#include <string.h>
#include <bsd/sys/queue.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>

#include <Scintilla.h>

#include "sciteco.h"
#include "interface.h"
#include "undo.h"
#include "qregisters.h"
#include "parser.h"

/*
 * Auxiliary functions
 */
static inline bool
is_glob_pattern(const gchar *str)
{
	return strchr(str, '*') || strchr(str, '?');
}

/*
 * Get absolute/full version of a possibly relative path.
 * Works with existing and non-existing paths (in the latter case,
 * heuristics may be applied.)
 */
gchar *get_absolute_path(const gchar *path);

/*
 * Classes
 */

class Buffer {
	class UndoTokenClose : public UndoToken {
		Buffer *buffer;

	public:
		UndoTokenClose(Buffer *_buffer)
			      : UndoToken(), buffer(_buffer) {}

		void run(void);
	};

public:
	TAILQ_ENTRY(Buffer) buffers;

	gchar *filename;
	gint dot;

	gint savepoint_id;

	bool dirty;

private:
	typedef void document;
	document *doc;

public:
	Buffer() : filename(NULL), dot(0), savepoint_id(0), dirty(false)
	{
		doc = (document *)interface.ssm(SCI_CREATEDOCUMENT);
	}
	~Buffer()
	{
		interface.ssm(SCI_RELEASEDOCUMENT, 0, (sptr_t)doc);
		g_free(filename);
	}

	inline Buffer *&
	next(void)
	{
		return TAILQ_NEXT(this, buffers);
	}
	inline Buffer *&
	prev(void)
	{
		TAILQ_HEAD(Head, Buffer);

		return TAILQ_PREV(this, Head, buffers);
	}

	inline void
	set_filename(const gchar *filename)
	{
		gchar *resolved = get_absolute_path(filename);
		g_free(Buffer::filename);
		Buffer::filename = resolved;
		interface.info_update(this);
	}

	inline void
	edit(void)
	{
		interface.ssm(SCI_SETDOCPOINTER, 0, (sptr_t)doc);
		interface.ssm(SCI_GOTOPOS, dot);
		interface.info_update(this);
	}
	inline void
	undo_edit(void)
	{
		interface.undo_info_update(this);
		undo.push_msg(SCI_GOTOPOS, dot);
		undo.push_msg(SCI_SETDOCPOINTER, 0, (sptr_t)doc);
	}

	bool load(const gchar *filename);

	inline void
	undo_close(void)
	{
		undo.push(new UndoTokenClose(this));
	}
};

extern class Ring {
	/*
	 * Emitted after a buffer close
	 * The pointer is the only remaining reference to the buffer!
	 */
	class UndoTokenEdit : public UndoToken {
		Ring	*ring;
		Buffer	*buffer;

	public:
		UndoTokenEdit(Ring *_ring, Buffer *_buffer)
			     : UndoToken(), ring(_ring), buffer(_buffer) {}
		~UndoTokenEdit()
		{
			if (buffer)
				delete buffer;
		}

		void run(void);
	};

	class UndoTokenRemoveFile : public UndoToken {
		gchar *filename;

	public:
		UndoTokenRemoveFile(const gchar *_filename)
				   : filename(g_strdup(_filename)) {}
		~UndoTokenRemoveFile()
		{
			g_free(filename);
		}

		void
		run(void)
		{
			g_unlink(filename);
		}
	};

	TAILQ_HEAD(Head, Buffer) head;

public:
	Buffer *current;

	Ring() : current(NULL)
	{
		TAILQ_INIT(&head);
	}
	~Ring();

	inline Buffer *
	first(void)
	{
		return TAILQ_FIRST(&head);
	}
	inline Buffer *
	last(void)
	{
		return TAILQ_LAST(&head, Head);
	}

	Buffer *find(const gchar *filename);
	Buffer *find(gint64 id);

	void dirtify(void);
	bool is_any_dirty(void);

	bool edit(gint64 id);
	void edit(const gchar *filename);
	inline void
	undo_edit(void)
	{
		current->dot = interface.ssm(SCI_GETCURRENTPOS);
		undo.push_var(current);
		current->undo_edit();
	}

	bool save(const gchar *filename);

	void close(Buffer *buffer);
	void close(void);
	inline void
	undo_close(void)
	{
		current->undo_close();
	}
} ring;

/*
 * Command states
 */

class StateEditFile : public StateExpectString {
private:
	bool allowFilename;

	void do_edit(const gchar *filename) throw (Error);
	void do_edit(gint64 id) throw (Error);

	void initial(void) throw (Error);
	State *done(const gchar *str) throw (Error);
};

class StateSaveFile : public StateExpectString {
private:
	State *done(const gchar *str) throw (Error);
};

namespace States {
	extern StateEditFile	editfile;
	extern StateSaveFile	savefile;
}

/* FIXME: clean up current_save_dot() usage */
static inline void
current_save_dot(void)
{
	gint dot = interface.ssm(SCI_GETCURRENTPOS);

	if (ring.current)
		ring.current->dot = dot;
	else if (QRegisters::current)
		QRegisters::current->dot = dot;
}

#endif
#ifndef __SEARCH_H
#define __SEARCH_H

#include <glib.h>

#include "sciteco.h"
#include "parser.h"
#include "qbuffers.h"

/*
 * "S" command state and base class for all other search/replace commands
 */
class StateSearch : public StateExpectString {
public:
	StateSearch(bool last = true) : StateExpectString(true, last) {}

protected:
	struct Parameters {
		gint dot;
		gint from, to;
		gint count;

		Buffer *from_buffer, *to_buffer;
	} parameters;

	enum MatchState {
		STATE_START,
		STATE_NOT,
		STATE_CTL_E,
		STATE_ANYQ,
		STATE_MANY,
		STATE_ALT
	};

	gchar *class2regexp(MatchState &state, const gchar *&pattern,
			    bool escape_default = false);
	gchar *pattern2regexp(const gchar *&pattern, bool single_expr = false);
	void do_search(GRegex *re, gint from, gint to, gint &count);

	virtual void initial(void) throw (Error);
	virtual void process(const gchar *str, gint new_chars) throw (Error);
	virtual State *done(const gchar *str) throw (Error);
};

class StateSearchAll : public StateSearch {
private:
	void initial(void) throw (Error);
	State *done(const gchar *str) throw (Error);
};

class StateSearchDelete : public StateSearch {
public:
	StateSearchDelete(bool last = true) : StateSearch(last) {}

protected:
	State *done(const gchar *str) throw (Error);
};

class StateReplace : public StateSearchDelete {
public:
	StateReplace() : StateSearchDelete(false) {}

private:
	State *done(const gchar *str) throw (Error);
};

class StateReplace_insert : public StateInsert {
private:
	void initial(void) throw (Error) {}
};

class StateReplaceDefault : public StateSearchDelete {
public:
	StateReplaceDefault() : StateSearchDelete(false) {}

private:
	State *done(const gchar *str) throw (Error);
};

class StateReplaceDefault_insert : public StateInsert {
private:
	void initial(void) throw (Error) {}
	State *done(const gchar *str) throw (Error);
};

namespace States {
	extern StateSearch			search;
	extern StateSearchAll			searchall;
	extern StateSearchDelete		searchdelete;
	extern StateReplace			replace;
	extern StateReplace_insert		replace_insert;
	extern StateReplaceDefault		replacedefault;
	extern StateReplaceDefault_insert	replacedefault_insert;
}

#endif

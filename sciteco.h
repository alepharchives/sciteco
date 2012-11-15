#ifndef __SCITECO_H
#define __SCITECO_H

#include <glib.h>

#include "interface.h"

extern gchar *cmdline;
extern bool quit_requested;

void cmdline_keypress(gchar key);

#define IS_CTL(C)	((C) < ' ')
#define CTL_ECHO(C)	((C) | 0x40)
#define CTL_KEY(C)	((C) & ~0x40)

typedef gint64 tecoBool;

#define SUCCESS		(-1)
#define FAILURE		(0)
#define TECO_BOOL(X)	((X) ? SUCCESS : FAILURE)

#define IS_SUCCESS(X)	((X) < 0)
#define IS_FAILURE(X)	(!IS_SUCCESS(X))

namespace String {

static inline void
append(gchar *&str1, const gchar *str2)
{
	/* FIXME: optimize */
	gchar *new_str = g_strconcat(str1 ? : "", str2, NULL);
	g_free(str1);
	str1 = new_str;
}

static inline void
append(gchar *&str, gchar chr)
{
	append(str, (gchar []){chr, '\0'});
}

} /* namespace String */

namespace Validate {

static inline bool
pos(gint n)
{
	return n >= 0 && n <= interface.ssm(SCI_GETLENGTH);
}

static inline bool
line(gint n)
{
	return n >= 0 && n < interface.ssm(SCI_GETLINECOUNT);
}

} /* namespace Validate */

#endif

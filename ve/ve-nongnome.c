/* GNOME dependant routines without use of gnome libs
 *
 * (c) 2000 Eazel, Inc.
 * (c) 2001,2002 George Lebl
 * (c) 2003 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <config.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "ve-i18n.h"

#include "ve-misc.h"

char *
ve_find_file (const char *filename, const GList *directories)
{
	char *s;
	const char *path;
	char **pathv;
	int i;

	s = ve_find_file_simple (filename, directories);
	if (s != NULL)
		return s;
	/* if the filename was NULL or absolute
	 * no further checks are made */
	if (filename == NULL ||
	    g_path_is_absolute (filename))
		return NULL;

	/* Can't use gnome_program here */

	path = g_getenv ("GNOME_PATH");
	if (path != NULL) {
		pathv = g_strsplit (path, ":", 0);
		for (i = 0; pathv[i] != NULL; i++) {
			s = g_build_filename (pathv[i], "share", filename, NULL);
			if (access (s, F_OK) == 0) {
				g_strfreev (pathv);
				return s;
			}
			g_free (s);
			s = g_build_filename (pathv[i], "share",
					      g_get_prgname (),
					      filename, NULL);
			if (access (s, F_OK) == 0) {
				g_strfreev (pathv);
				return s;
			}
			g_free (s);
		}
		g_strfreev (pathv);
	}

	return NULL;
}

/* Following stolen from libgnome / gnome-i18n.c */

static GHashTable *alias_table = NULL;

/*read an alias file for the locales*/
static void
read_aliases (char *file)
{
  FILE *fp;
  char buf[256];
  if (!alias_table)
    alias_table = g_hash_table_new (g_str_hash, g_str_equal);
  VE_IGNORE_EINTR (fp = fopen (file,"r"));
  if (!fp)
    return;
  for (;;)
    {
      char *p;
      VE_IGNORE_EINTR (p = fgets (buf, sizeof (buf), fp));
      if (p == NULL)
	      break;
      g_strstrip(buf);
      if (buf[0]=='#' || buf[0]=='\0')
        continue;
      p = strtok (buf,"\t ");
      if (!p)
	continue;
      p = strtok (NULL,"\t ");
      if(!p)
	continue;
      if (!g_hash_table_lookup (alias_table, buf))
	g_hash_table_insert (alias_table, g_strdup(buf), g_strdup(p));
    }
  VE_IGNORE_EINTR (fclose (fp));
}

/*return the un-aliased language as a newly allocated string*/
static char *
unalias_lang (char *lang)
{
  char *p;
  int i;
  if (!alias_table)
    {
      read_aliases ("/usr/share/locale/locale.alias");
      read_aliases ("/usr/local/share/locale/locale.alias");
      read_aliases ("/usr/lib/X11/locale/locale.alias");
      read_aliases ("/usr/openwin/lib/locale/locale.alias");
    }
  i = 0;
  while ((p=g_hash_table_lookup(alias_table,lang)) && strcmp(p, lang))
    {
      lang = p;
      if (i++ == 30)
        {
          static gboolean said_before = FALSE;
	  if (!said_before)
            g_warning (_("Too many alias levels for a locale; "
			 "may indicate a loop"));
	  said_before = TRUE;
	  return lang;
	}
    }
  return lang;
}

/* Mask for components of locale spec. The ordering here is from
 * least significant to most significant
 */
enum
{
  COMPONENT_CODESET =   1 << 0,
  COMPONENT_TERRITORY = 1 << 1,
  COMPONENT_MODIFIER =  1 << 2
};

/* Break an X/Open style locale specification into components
 */
static guint
explode_locale (const gchar *locale,
		gchar **language, 
		gchar **territory, 
		gchar **codeset, 
		gchar **modifier)
{
  const gchar *uscore_pos;
  const gchar *at_pos;
  const gchar *dot_pos;

  guint mask = 0;

  uscore_pos = strchr (locale, '_');
  dot_pos = strchr (uscore_pos ? uscore_pos : locale, '.');
  at_pos = strchr (dot_pos ? dot_pos : (uscore_pos ? uscore_pos : locale), '@');

  if (at_pos)
    {
      mask |= COMPONENT_MODIFIER;
      *modifier = g_strdup (at_pos);
    }
  else
    at_pos = locale + strlen (locale);

  if (dot_pos)
    {
      mask |= COMPONENT_CODESET;
      *codeset = g_new (gchar, 1 + at_pos - dot_pos);
      strncpy (*codeset, dot_pos, at_pos - dot_pos);
      (*codeset)[at_pos - dot_pos] = '\0';
    }
  else
    dot_pos = at_pos;

  if (uscore_pos)
    {
      mask |= COMPONENT_TERRITORY;
      *territory = g_new (gchar, 1 + dot_pos - uscore_pos);
      strncpy (*territory, uscore_pos, dot_pos - uscore_pos);
      (*territory)[dot_pos - uscore_pos] = '\0';
    }
  else
    uscore_pos = dot_pos;

  *language = g_new (gchar, 1 + uscore_pos - locale);
  strncpy (*language, locale, uscore_pos - locale);
  (*language)[uscore_pos - locale] = '\0';

  return mask;
}

/*
 * Compute all interesting variants for a given locale name -
 * by stripping off different components of the value.
 *
 * For simplicity, we assume that the locale is in
 * X/Open format: language[_territory][.codeset][@modifier]
 *
 * TODO: Extend this to handle the CEN format (see the GNUlibc docs)
 *       as well. We could just copy the code from glibc wholesale
 *       but it is big, ugly, and complicated, so I'm reluctant
 *       to do so when this should handle 99% of the time...
 */
static GList *
compute_locale_variants (const gchar *locale)
{
  GList *retval = NULL;

  gchar *language;
  gchar *territory;
  gchar *codeset;
  gchar *modifier;

  guint mask;
  guint i;

  g_return_val_if_fail (locale != NULL, NULL);

  mask = explode_locale (locale, &language, &territory, &codeset, &modifier);

  /* Iterate through all possible combinations, from least attractive
   * to most attractive.
   */
  for (i=0; i<=mask; i++)
    if ((i & ~mask) == 0)
      {
	gchar *val = g_strconcat(language,
				 (i & COMPONENT_TERRITORY) ? territory : "",
				 (i & COMPONENT_CODESET) ? codeset : "",
				 (i & COMPONENT_MODIFIER) ? modifier : "",
				 NULL);
	retval = g_list_prepend (retval, val);
      }

  g_free (language);
  if (mask & COMPONENT_CODESET)
    g_free (codeset);
  if (mask & COMPONENT_TERRITORY)
    g_free (territory);
  if (mask & COMPONENT_MODIFIER)
    g_free (modifier);

  return retval;
}

/* The following is (partly) taken from the gettext package.
   Copyright (C) 1995, 1996, 1997, 1998 Free Software Foundation, Inc.  */

static const gchar *
guess_category_value (const gchar *categoryname)
{
  const gchar *retval;

  /* The highest priority value is the `LANGUAGE' environment
     variable.  This is a GNU extension.  */
  retval = g_getenv ("LANGUAGE");
  if (retval != NULL && retval[0] != '\0')
    return retval;

  /* `LANGUAGE' is not set.  So we have to proceed with the POSIX
     methods of looking to `LC_ALL', `LC_xxx', and `LANG'.  On some
     systems this can be done by the `setlocale' function itself.  */

  /* Setting of LC_ALL overwrites all other.  */
  retval = g_getenv ("LC_ALL");  
  if (retval != NULL && retval[0] != '\0')
    return retval;

  /* Next comes the name of the desired category.  */
  retval = g_getenv (categoryname);
  if (retval != NULL && retval[0] != '\0')
    return retval;

  /* Last possibility is the LANG environment variable.  */
  retval = g_getenv ("LANG");
  if (retval != NULL && retval[0] != '\0')
    return retval;

  return NULL;
}


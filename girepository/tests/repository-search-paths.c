/*
 * Copyright 2023 Canonical Ltd.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Marco Trevisan <marco.trevisan@canonical.com>
 */

#include "glib.h"
#include "girepository.h"

/* Keep this test first, not to add search paths to other tests */
static void
test_repository_search_paths_unset (void)
{
  const char * const *search_paths;
  size_t n_search_paths;

  search_paths = gi_repository_get_search_path (&n_search_paths);
  g_assert_nonnull (search_paths);
  g_assert_cmpstrv (search_paths, ((char *[]){NULL}));
  g_assert_cmpuint (n_search_paths, ==, 0);

  search_paths = gi_repository_get_search_path (NULL);
  g_assert_cmpuint (g_strv_length ((char **) search_paths), ==, 0);
}

static void
test_repository_search_paths_default (void)
{
  const char * const *search_paths;
  size_t n_search_paths;

  search_paths = gi_repository_get_search_path (&n_search_paths);
  g_assert_nonnull (search_paths);

  /* Init default paths */
  g_assert_nonnull (gi_repository_get_default ());

  search_paths = gi_repository_get_search_path (&n_search_paths);
  g_assert_nonnull (search_paths);
  g_assert_cmpuint (g_strv_length ((char **) search_paths), ==, 2);

  g_assert_cmpstr (search_paths[0], ==, g_get_tmp_dir ());

#ifndef G_PLATFORM_WIN32
  char *expected_path = g_build_filename (GOBJECT_INTROSPECTION_LIBDIR, "girepository-1.0", NULL);
  g_assert_cmpstr (search_paths[1], ==, expected_path);
  g_clear_pointer (&expected_path, g_free);
#endif
}

static void
test_repository_search_paths_prepend (void)
{
  const char * const *search_paths;
  size_t n_search_paths;

  gi_repository_prepend_search_path (g_test_get_dir (G_TEST_BUILT));
  search_paths = gi_repository_get_search_path (&n_search_paths);
  g_assert_nonnull (search_paths);
  g_assert_cmpuint (g_strv_length ((char **) search_paths), ==, 3);

  g_assert_cmpstr (search_paths[0], ==, g_test_get_dir (G_TEST_BUILT));
  g_assert_cmpstr (search_paths[1], ==, g_get_tmp_dir ());

#ifndef G_PLATFORM_WIN32
  char *expected_path = g_build_filename (GOBJECT_INTROSPECTION_LIBDIR, "girepository-1.0", NULL);
  g_assert_cmpstr (search_paths[2], ==, expected_path);
  g_clear_pointer (&expected_path, g_free);
#endif

  gi_repository_prepend_search_path (g_test_get_dir (G_TEST_DIST));
  search_paths = gi_repository_get_search_path (&n_search_paths);
  g_assert_nonnull (search_paths);
  g_assert_cmpuint (g_strv_length ((char **) search_paths), ==, 4);

  g_assert_cmpstr (search_paths[0], ==, g_test_get_dir (G_TEST_DIST));
  g_assert_cmpstr (search_paths[1], ==, g_test_get_dir (G_TEST_BUILT));
  g_assert_cmpstr (search_paths[2], ==, g_get_tmp_dir ());

#ifndef G_PLATFORM_WIN32
  expected_path = g_build_filename (GOBJECT_INTROSPECTION_LIBDIR, "girepository-1.0", NULL);
  g_assert_cmpstr (search_paths[3], ==, expected_path);
  g_clear_pointer (&expected_path, g_free);
#endif
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, G_TEST_OPTION_ISOLATE_DIRS, NULL);

  /* Isolate from the system typelibs and GIRs. */
  g_setenv ("GI_TYPELIB_PATH", g_get_tmp_dir (), TRUE);
  g_setenv ("GI_GIR_PATH", g_get_user_cache_dir (), TRUE);

  g_test_add_func ("/repository-search-paths/unset", test_repository_search_paths_unset);
  g_test_add_func ("/repository-search-paths/default", test_repository_search_paths_default);
  g_test_add_func ("/repository-search-paths/prepend", test_repository_search_paths_prepend);

  return g_test_run ();
}

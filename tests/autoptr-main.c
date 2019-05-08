/* autoptr-main.c
 *
 * Copyright 2019 Leesoo Ahn <yisooan@fedoraproject.org>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <gairq/gairq.h>

#include <locale.h>

static void
test_gairq_air_object (gconstpointer token)
{
  g_autoptr(GairqAirObject) val = NULL;

  val = g_object_new (GAIRQ_TYPE_AIR_OBJECT, NULL);
  g_assert_nonnull (val);
}

static void
test_gairq_city (gconstpointer token)
{
  g_autoptr(GairqCity) val = NULL;

  val = gairq_city_new (token,
                        GAIRQ_CITY_TYPE_NAME,
                        "");
  g_assert_nonnull (val);
}

static void
test_gairq_request (gconstpointer token)
{
  g_autoptr(GairqRequest) val = NULL;

  val = gairq_request_new (token);
  g_assert_nonnull (val);
}

int
main (int   argc,
      char *argv[])
{
  setlocale (LC_CTYPE, "");

  g_test_init (&argc, &argv, NULL);

  const gchar *token = g_getenv ("GAIRQ_API_TOKEN");
  g_assert_nonnull (token);

  g_test_add_data_func ("/Gairq/autoptr/AirObject",
                        token,
                        test_gairq_air_object);

  g_test_add_data_func ("/Gairq/autoptr/City",
                        token,
                        test_gairq_city);

  g_test_add_data_func ("/Gairq/autoptr/Request",
                        token,
                        test_gairq_request);

  return g_test_run ();
}

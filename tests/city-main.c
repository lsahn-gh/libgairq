/* city-main.c
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

#include <stdio.h>
#include <locale.h>
#include <math.h>

static void
test_gairq_with_name_request (gconstpointer token)
{
  g_autoptr(GairqCity) instance = NULL;
  g_autoptr(GError) error = NULL;
  g_autoptr(GairqAirObject) air = NULL;
  GHashTable *iaqi = NULL;
  gdouble value;

  instance = gairq_city_new_with_name (token, "istanbul");
  g_assert_nonnull (instance);

  air = gairq_city_request_sync (instance, &error);
  g_assert_no_error (error);

  g_assert (gairq_air_object_get_idx (air) == 4143);
  g_assert (gairq_air_object_get_aqi (air) == -1);
  value = gairq_air_object_get_city (air)->geo.latitude;
  g_assert (fabs (value - 41.014722) < DBL_EPSILON);
  value = gairq_air_object_get_city (air)->geo.longitude;
  g_assert (fabs (value - 28.954722) < DBL_EPSILON);

  iaqi = gairq_air_object_get_iaqi (air);
  g_assert (g_hash_table_contains (iaqi, "co"));
  g_assert (g_hash_table_contains (iaqi, "pm10"));
  g_assert (g_hash_table_contains (iaqi, "pm25"));
  g_assert (g_hash_table_contains (iaqi, "o3"));
}

int
main (int   argc,
      char *argv[])
{
  setlocale (LC_CTYPE, "");

  g_test_init (&argc, &argv, NULL);

  const gchar *token = g_getenv ("GAIRQ_API_TOKEN");
  g_assert_nonnull (token);

  g_test_add_data_func ("/Gairq/city/request/sync",
                        token,
                        test_gairq_with_name_request);

  return g_test_run ();
}

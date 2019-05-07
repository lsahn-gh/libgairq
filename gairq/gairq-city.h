/* gairq-city.h
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

#ifndef GAIRQ_CITY_H
#define GAIRQ_CITY_H

#if !defined(GAIRQ_INSIDE) && !defined(GAIRQ_COMPILATION)
# error "Only <gairq/gairq.h> can be included directly."
#endif

#include <glib-object.h>
#include <gairq/gairq-air-object.h>
#include <gairq/gairq-request.h>

G_BEGIN_DECLS

#define GAIRQ_TYPE_CITY (gairq_city_get_type ())
G_DECLARE_FINAL_TYPE (GairqCity, gairq_city, GAIRQ, CITY, GairqRequest)

typedef enum {
  GAIRQ_CITY_TYPE_NONE,
  GAIRQ_CITY_TYPE_NAME,
  GAIRQ_CITY_TYPE_ID,
  N_GAIRQ_CITY_TYPES
} GairqCityType;

GairqCity *       gairq_city_new              (const gchar   *access_token,
                                               GairqCityType  city_type,
                                               const gchar   *city);
GairqCity *       gairq_city_new_with_name    (const gchar *access_token,
                                               const gchar *city_name);
GairqCity *       gairq_city_new_with_id      (const gchar *access_token,
                                               const gchar *city_id);
GairqAirObject *  gairq_city_request_sync     (GairqCity  *self,
                                               GError    **error);
void              gairq_city_request_async    (GairqCity           *self,
                                               GCancellable        *cancellable,
                                               GAsyncReadyCallback  callback,
                                               gpointer             callback_data);
GairqAirObject *  gairq_city_request_finish   (GairqCity     *self,
                                               GAsyncResult  *res,
                                               GError       **error);

G_END_DECLS

#endif

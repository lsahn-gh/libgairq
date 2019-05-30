/* gairq-geo.h
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

#ifndef GAIRQ_GEO_H
#define GAIRQ_GEO_H

#if !defined(GAIRQ_INSIDE) && !defined(GAIRQ_COMPILATION)
# error "Only <gairq/gairq.h> can be included directly."
#endif

#include <glib-object.h>
#include <gairq/gairq-air-object.h>
#include <gairq/gairq-request.h>

G_BEGIN_DECLS

#define GAIRQ_TYPE_GEO (gairq_geo_get_type ())
G_DECLARE_FINAL_TYPE (GairqGeo, gairq_geo, GAIRQ, GEO, GairqRequest)

typedef enum {
  GAIRQ_GEO_TYPE_NONE,
  GAIRQ_GEO_TYPE_IP,
  GAIRQ_GEO_TYPE_LATLNG,
  N_GAIRQ_GEO_TYPES
} GairqGeoType;

GairqGeo *        gairq_geo_new_with_ip       (const gchar *access_token);
GairqGeo *        gairq_geo_new_with_lat_lng  (const gchar *access_token,
                                               const gchar *lat,
                                               const gchar *lng);
GairqAirObject *  gairq_geo_request_sync      (GairqGeo  *self,
                                               GError   **error);
void              gairq_geo_request_async     (GairqGeo            *self,
                                               GCancellable        *cancellable,
                                               GAsyncReadyCallback  callback,
                                               gpointer             callback_data);
GairqAirObject *  gairq_geo_request_finish    (GairqGeo      *self,
                                               GAsyncResult  *res,
                                               GError       **error);

G_END_DECLS

#endif

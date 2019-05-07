/* gairq-air-object.h
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

#ifndef GAIRQ_AIR_OBJECT_H
#define GAIRQ_AIR_OBJECT_H

#if !defined(GAIRQ_INSIDE) && !defined(GAIRQ_COMPILATION)
# error "Only <gairq/gairq.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define GAIRQ_TYPE_AIR_OBJECT (gairq_air_object_get_type ())
G_DECLARE_FINAL_TYPE (GairqAirObject, gairq_air_object, GAIRQ, AIR_OBJECT, GObject)

typedef struct _GairqObjectAttr GairqObjectAttr;
typedef struct _GairqObjectCity GairqObjectCity;

struct _GairqObjectAttr
{
  gchar *name;
  gchar *url;
};

struct _GairqObjectCity
{
  gchar *name;
  gchar *url;
  struct geo
    {
      gdouble latitude;
      gdouble longitude;
    } geo;
};


/* --- GairqAirObject --- */
gint64            gairq_air_object_get_idx          (GairqAirObject *self);
gint64            gairq_air_object_get_aqi          (GairqAirObject *self);
GSList *          gairq_air_object_get_attributions (GairqAirObject *self);
GairqObjectCity * gairq_air_object_get_city         (GairqAirObject *self);
GHashTable *      gairq_air_object_get_iaqi         (GairqAirObject *self);

/* --- GairqObjectAttr --- */
GairqObjectAttr *   gairq_object_attr_new       (const gchar *name,
                                                 const gchar *url);
void                gairq_object_attr_free      (GairqObjectAttr *attr);
GairqObjectAttr *   gairq_object_attr_copy      (GairqObjectAttr *src);
GairqObjectAttr *   gairq_object_attr_copy_deep (GairqObjectAttr *src);

/* --- GairqObjectCity --- */
GairqObjectCity *   gairq_object_city_new       (const gchar *name,
                                                 const gchar *url,
                                                 gdouble      lat,
                                                 gdouble      lon);
void                gairq_object_city_free      (GairqObjectCity *city);
GairqObjectCity *   gairq_object_city_copy      (GairqObjectCity *src);
GairqObjectCity *   gairq_object_city_copy_deep (GairqObjectCity *src);

/* --- GHashTable typed iaqi property --- */
GHashTable *        gairq_object_iaqi_new       (void);
void                gairq_object_iaqi_free      (GHashTable *iaqi);
G_END_DECLS

#endif

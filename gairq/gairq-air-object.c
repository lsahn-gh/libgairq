/* gairq-air-object.c
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

#include "gairq-air-object.h"
#include "gairq-debug.h"

#include <json-glib/json-glib.h>

struct _GairqAirObject
{
  GObject           parent_instance;

  gint64            idx;
  gint64            aqi;
  GSList *          attrs;
  GairqObjectCity * city;
  GHashTable *      iaqi;
};

enum {
  PROP_0,
  PROP_IDX,
  PROP_AQI,
  PROP_ATTRS,
  PROP_CITY,
  PROP_IAQI,
  N_PROPERTIES
};

GParamSpec* properties [N_PROPERTIES];

static void gairq_air_object_json_serializable_iface_init (JsonSerializableIface *iface);

G_DEFINE_TYPE_WITH_CODE (GairqAirObject, gairq_air_object, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (JSON_TYPE_SERIALIZABLE,
                                                gairq_air_object_json_serializable_iface_init))


/* --- GObject --- */
static void
gairq_air_object_finalize (GObject *object)
{
  GairqAirObject *self = GAIRQ_AIR_OBJECT (object);

  g_slist_free_full (self->attrs,
                     (GDestroyNotify) gairq_object_attr_free);

  gairq_object_city_free (self->city);
  gairq_object_iaqi_free (self->iaqi);

  G_OBJECT_CLASS (gairq_air_object_parent_class)->finalize (object);
}

static void
gairq_air_object_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  GairqAirObject *self = GAIRQ_AIR_OBJECT (object);

  switch (prop_id)
    {
    case PROP_IDX:
      self->idx = g_value_get_int64 (value);
      break;

    case PROP_AQI:
      self->aqi = g_value_get_int64 (value);
      break;

    case PROP_ATTRS:
      g_slist_free_full (self->attrs,
                         (GDestroyNotify) gairq_object_attr_free);
      self->attrs = g_value_get_pointer (value);
      break;

    case PROP_CITY:
      gairq_object_city_free (self->city);
      self->city = g_value_get_pointer (value);
      break;

    case PROP_IAQI:
      gairq_object_iaqi_free (self->iaqi);
      self->iaqi = g_value_get_pointer (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gairq_air_object_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  GairqAirObject *self = GAIRQ_AIR_OBJECT (object);

  switch (prop_id)
    {
    case PROP_IDX:
      g_value_set_int64 (value, self->idx);
      break;

    case PROP_AQI:
      g_value_set_int64 (value, self->aqi);
      break;

    case PROP_ATTRS:
      g_value_set_pointer (value, self->attrs);
      break;

    case PROP_CITY:
      g_value_set_pointer (value, self->city);
      break;

    case PROP_IAQI:
      g_value_set_pointer (value, self->iaqi);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gairq_air_object_class_init (GairqAirObjectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gairq_air_object_finalize;
  object_class->set_property = gairq_air_object_set_property;
  object_class->get_property = gairq_air_object_get_property;

  properties [PROP_IDX] =
    g_param_spec_int64 ("idx", "Idx",
                        "A unique ID for the city monitoring station",
                        G_MININT64, G_MAXINT64, 0,
                        G_PARAM_READWRITE);

  properties [PROP_AQI] =
    g_param_spec_int64 ("aqi", "Aqi",
                        "Real-time air quality information",
                        G_MININT64, G_MAXINT64, 0,
                        G_PARAM_READWRITE);

  properties [PROP_ATTRS] =
    g_param_spec_pointer ("attributions", "Attributions",
                          "EPA Attributions for the station",
                          G_PARAM_READWRITE);

  properties [PROP_CITY] =
    g_param_spec_pointer ("city", "City",
                          "Information about the monitoring station",
                          G_PARAM_READWRITE);

  properties [PROP_IAQI] =
    g_param_spec_pointer ("iaqi", "Iaqi",
                          "Measurement time information",
                          G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gairq_air_object_init (GairqAirObject *self)
{
  self->attrs = NULL;
  self->city = NULL;
  self->iaqi = NULL;
}

/* --- JsonSerializableIface --- */
static JsonNode *
gairq_air_object_json_serialize_property (JsonSerializable *serializable,
                                          const gchar      *property_name,
                                          const GValue     *value,
                                          GParamSpec       *pspec)
{
  gpointer ret;

  /* Ever since the API provides only GET method, we have nothing to do */
  if (g_strcmp0 ("attributions", property_name) == 0 ||
      g_strcmp0 ("city", property_name) == 0 ||
      g_strcmp0 ("iaqi", property_name) == 0)
    {
      ret = NULL;
    }
  else
    {
      ret = json_serializable_default_serialize_property (serializable,
                                                          property_name,
                                                          value,
                                                          pspec);
    }

  return ret;
}

static gboolean
gairq_air_object_json_deserialize_property (JsonSerializable *serializable,
                                            const gchar      *property_name,
                                            GValue           *value,
                                            GParamSpec       *pspec,
                                            JsonNode         *property_node)
{
  gboolean ret = FALSE;

  if (g_strcmp0 ("aqi", property_name) == 0)
    {
      if (JSON_NODE_HOLDS_VALUE (property_node))
        {
          const gchar *node_type;
          gint64 real_value = -1;

          /* We expect the value of "aqi" is gint64 type, but the API server
           * rarely returns the value, dash(-) as string in which means
           * it has nothing. It happens on, for e.g "Istanbul" city.
           *
           * To avoid core dump by the above happening, it checks
           * the type of the value of the "aqi" node every time.
           */
          node_type = json_node_type_name (property_node);
          if (g_strcmp0 ("Integer", node_type) == 0)
            real_value = json_node_get_int (property_node);
          else
            gairq_debug ("Failed to parse a value: aqi value is not the Integer type");

          g_value_set_int64 (value, real_value);
          ret = TRUE;
        }
    }

  else if (g_strcmp0 ("attributions", property_name) == 0)
    {
      if (JSON_NODE_HOLDS_ARRAY (property_node))
        {
          JsonArray *jarr;
          GList *list;
          GSList *attrs = NULL;

          jarr = json_node_get_array (property_node);
          list = json_array_get_elements (jarr);

          for ( ; list; list = g_list_next (list))
            {
              JsonObject *elem;
              const gchar *name, *url;
              GairqObjectAttr *new_attr;

              elem = json_node_get_object ((JsonNode*) list->data);
              name = json_object_get_string_member (elem, "name");
              url = json_object_get_string_member (elem, "url");

              new_attr = gairq_object_attr_new (name, url);
              attrs = g_slist_append (attrs, new_attr);
            }

          g_value_set_pointer (value, attrs);
          ret = TRUE;
        }
    }

  else if (g_strcmp0 ("city", property_name) == 0)
    {
      if (JSON_NODE_HOLDS_OBJECT (property_node))
        {
          JsonObject *city_obj;
          const gchar *name, *url;
          JsonArray *geo;

          city_obj = json_node_get_object (property_node);
          name = json_object_get_string_member (city_obj, "name");
          url = json_object_get_string_member (city_obj, "url");
          geo = json_object_get_array_member (city_obj, "geo");

          if (json_array_get_length (geo) == 2)
            {
              gdouble lat, lng;
              GairqObjectCity *new_city;

              lat = json_array_get_double_element (geo, 0);
              lng = json_array_get_double_element (geo, 1);
              new_city = gairq_object_city_new (name, url, lat, lng);

              g_value_set_pointer (value, new_city);
              ret = TRUE;
            }
        }
    }

  else if (g_strcmp0 ("iaqi", property_name) == 0)
    {
      if (JSON_NODE_HOLDS_OBJECT (property_node))
        {
          JsonObject *iaqi_object;
          GHashTable *new_iaqi;
          GList *elem;

          new_iaqi = gairq_object_iaqi_new ();

          iaqi_object = json_node_get_object (property_node);
          elem = json_object_get_members (iaqi_object);

          for ( ; elem; elem = g_list_next (elem))
            {
              gchar *key;
              gdouble real_number;
              gpointer value;

              key = g_strdup (elem->data);

              /* As JsonNode */
              value = json_object_get_member (iaqi_object, key);
              /* As JsonObject */
              value = json_node_get_object (value);
              real_number = json_object_get_double_member (value, "v");

              value = g_slice_new0 (gdouble);
              *(gdouble*)value = real_number;

              g_hash_table_insert (new_iaqi, key, value);
            }

          g_value_set_pointer (value, new_iaqi);
          ret = TRUE;
        }
    }

  else
    {
      ret = json_serializable_default_deserialize_property (serializable,
                                                            property_name,
                                                            value,
                                                            pspec,
                                                            property_node);
    }

  return ret;
}

static GParamSpec *
gairq_air_object_json_find_property (JsonSerializable *serializable,
                                     const gchar      *name)
{
  return g_object_class_find_property (G_OBJECT_GET_CLASS (serializable), name);
}

static GParamSpec **
gairq_air_object_json_list_properties (JsonSerializable *serializable,
                                       guint            *n_pspecs)
{
  return g_object_class_list_properties (G_OBJECT_GET_CLASS (serializable), n_pspecs);
}

static void
gairq_air_object_json_set_property (JsonSerializable *serializable,
                                    GParamSpec       *pspec,
                                    const GValue     *value)
{
  g_object_set_property (G_OBJECT (serializable),
                         g_param_spec_get_name (pspec),
                         value);
}

static void
gairq_air_object_json_get_property (JsonSerializable *serializable,
                                    GParamSpec       *pspec,
                                    GValue           *value)
{
  g_object_get_property (G_OBJECT (serializable),
                         g_param_spec_get_name (pspec),
                         value);
}

static void
gairq_air_object_json_serializable_iface_init (JsonSerializableIface *iface)
{
  iface->serialize_property = gairq_air_object_json_serialize_property;
  iface->deserialize_property = gairq_air_object_json_deserialize_property;
  iface->find_property = gairq_air_object_json_find_property;
  iface->list_properties = gairq_air_object_json_list_properties;
  iface->set_property = gairq_air_object_json_set_property;
  iface->get_property = gairq_air_object_json_get_property;
}

/* --- GairqObjectAttr --- */
GairqObjectAttr *
gairq_object_attr_new (const gchar *name,
                       const gchar *url)
{
  GairqObjectAttr *new_attr;
  g_return_val_if_fail (name != NULL, NULL);
  g_return_val_if_fail (url != NULL, NULL);

  new_attr = g_slice_new0 (GairqObjectAttr);
  new_attr->name = g_strdup (name);
  new_attr->url = g_strdup (url);

  return new_attr;
}

void
gairq_object_attr_free (GairqObjectAttr *attr)
{
  if (attr)
    {
      g_free (attr->name);
      g_free (attr->url);
      g_slice_free (GairqObjectAttr, attr);
    }
}

GairqObjectAttr *
gairq_object_attr_copy (GairqObjectAttr *src)
{
  if (src == NULL)
    return NULL;

  return g_slice_dup (GairqObjectAttr, src);
}

GairqObjectAttr *
gairq_object_attr_copy_deep (GairqObjectAttr *src)
{
  if (src == NULL)
    return NULL;

  return gairq_object_attr_new (src->name, src->url);
}

/* --- GairqObjectCity --- */
GairqObjectCity *
gairq_object_city_new (const gchar *name,
                       const gchar *url,
                       gdouble      lat,
                       gdouble      lng)
{
  GairqObjectCity *new_city;
  g_return_val_if_fail (name != NULL, NULL);
  g_return_val_if_fail (url != NULL, NULL);

  new_city = g_slice_new0 (GairqObjectCity);
  new_city->name = g_strdup (name);
  new_city->url = g_strdup (url);
  new_city->geo.latitude = lat;
  new_city->geo.longitude = lng;

  return new_city;
}

void
gairq_object_city_free (GairqObjectCity *city)
{
  if (city)
    {
      g_free (city->name);
      g_free (city->url);
      g_slice_free (GairqObjectCity, city);
    }
}

GairqObjectCity *
gairq_object_city_copy (GairqObjectCity *src)
{
  if (src == NULL)
    return NULL;

  return g_slice_dup (GairqObjectCity, src);
}

GairqObjectCity *
gairq_object_city_copy_deep (GairqObjectCity *src)
{
  if (src == NULL)
    return NULL;

  return gairq_object_city_new (src->name,
                                src->url,
                                src->geo.latitude,
                                src->geo.longitude);
}

/* --- GHashTable for iaqi property --- */
static void
gdouble_slice_free_func (gpointer src)
{
  g_slice_free (gdouble, src);
}

GHashTable *
gairq_object_iaqi_new (void)
{
  GHashTable *new_ht;

  new_ht = g_hash_table_new_full (g_str_hash,
                                  g_str_equal,
                                  g_free,
                                  gdouble_slice_free_func);

  return new_ht;
}

void
gairq_object_iaqi_free (GHashTable *iaqi)
{
  if (iaqi)
    g_hash_table_destroy (iaqi);
}

/* --- GairqAirObject --- */
gint64
gairq_air_object_get_idx (GairqAirObject *self)
{
  g_return_val_if_fail (GAIRQ_IS_AIR_OBJECT (self), -1);

  return self->idx;
}

gint64
gairq_air_object_get_aqi (GairqAirObject *self)
{
  g_return_val_if_fail (GAIRQ_IS_AIR_OBJECT (self), -1);

  return self->aqi;
}

GSList *
gairq_air_object_get_attributions (GairqAirObject *self)
{
  g_return_val_if_fail (GAIRQ_IS_AIR_OBJECT (self), NULL);

  return self->attrs;
}

GairqObjectCity *
gairq_air_object_get_city (GairqAirObject *self)
{
  g_return_val_if_fail (GAIRQ_IS_AIR_OBJECT (self), NULL);

  return self->city;
}

GHashTable *
gairq_air_object_get_iaqi (GairqAirObject *self)
{
  g_return_val_if_fail (GAIRQ_IS_AIR_OBJECT (self), NULL);

  return self->iaqi;
}

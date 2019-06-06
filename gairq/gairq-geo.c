/* gairq-geo.c
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

#include "gairq-debug.h"
#include "gairq-geo.h"
#include "gairq-utils.h"

#include <json-glib/json-glib.h>
#include <rest/rest-proxy.h>

struct _GairqGeo
{
  GairqRequest  parent_instance;

  GairqGeoType  type;
  gchar *       lat;
  gchar *       lng;
};

/* Properties */
enum {
  PROP_0,
  PROP_TYPE,
  PROP_LATITUDE,
  PROP_LONGITUDE,
  N_PROPERTIES
};

static GParamSpec* properties [N_PROPERTIES];

G_DEFINE_TYPE (GairqGeo, gairq_geo, GAIRQ_TYPE_REQUEST)

#define GAIRQ_GEO_ERROR (gairq_geo_error_quark ())


/* --- GairqGeoError --- */
static GQuark
gairq_geo_error_quark (void)
{
  return g_quark_from_static_string ("gairq-geo-error-quark");
}

/* --- GObject --- */
static void
gairq_geo_finalize (GObject *object)
{
  GairqGeo *self = GAIRQ_GEO (object);

  g_free (self->lat);
  g_free (self->lng);

  G_OBJECT_CLASS (gairq_geo_parent_class)->finalize (object);
}

static void
gairq_geo_set_property (GObject      *object,
                        guint         prop_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
  GairqGeo *self = GAIRQ_GEO (object);

  switch (prop_id)
    {
    case PROP_TYPE:
      self->type = g_value_get_uint (value);
      break;

    case PROP_LATITUDE:
      g_free (self->lat);
      self->lat = g_value_dup_string (value);
      break;

    case PROP_LONGITUDE:
      g_free (self->lng);
      self->lng = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gairq_geo_get_property (GObject    *object,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
  GairqGeo *self = GAIRQ_GEO (object);

  switch (prop_id)
    {
    case PROP_TYPE:
      g_value_set_uint (value, self->type);
      break;

    case PROP_LATITUDE:
      g_value_set_string (value, self->lat);
      break;

    case PROP_LONGITUDE:
      g_value_set_string (value, self->lng);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static gboolean
gairq_geo_request_set_functions (RestProxyCall  *proxy_call,
                                 gpointer        user_data,
                                 GError        **error)
{
  GairqGeo *self = GAIRQ_GEO (user_data);
  gboolean ret = TRUE;
  gchar *functions = NULL;

  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  switch (self->type)
    {
    case GAIRQ_GEO_TYPE_IP:
      functions = g_strdup ("/feed/here/");
      break;

    case GAIRQ_GEO_TYPE_LATLNG:
      functions = g_strdup_printf ("/feed/geo:%s;%s/", self->lat, self->lng);
      break;

    default:
      if (error)
        g_set_error (error, GAIRQ_GEO_ERROR, 0,
                     "Invalid geo type code: %d", self->type);
      ret = FALSE;
    }

  if (ret)
    rest_proxy_call_set_function (proxy_call, functions);

  g_free (functions);

  return ret;
}

static gboolean
gairq_geo_request_set_parameters (RestProxyCall  *proxy_call,
                                  gpointer        user_data,
                                  GError        **error)
{
  return TRUE;
}

static void
gairq_geo_class_init (GairqGeoClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GairqRequestClass *request_class = GAIRQ_REQUEST_CLASS (klass);

  object_class->finalize = gairq_geo_finalize;
  object_class->set_property = gairq_geo_set_property;
  object_class->get_property = gairq_geo_get_property;

  request_class->set_functions = gairq_geo_request_set_functions;
  request_class->set_parameters = gairq_geo_request_set_parameters;

  properties [PROP_TYPE] =
    g_param_spec_uint ("type", "Type",
                       "A helper property for picking up a way of how to request",
                       GAIRQ_GEO_TYPE_NONE,
                       N_GAIRQ_GEO_TYPES,
                       GAIRQ_GEO_TYPE_NONE,
                       G_PARAM_READWRITE);

  properties [PROP_LATITUDE] =
    g_param_spec_string ("latitude", "Latitude",
                         "A latitude",
                         NULL,
                         G_PARAM_READWRITE);

  properties [PROP_LONGITUDE] =
    g_param_spec_string ("longitude", "Longitude",
                         "A longitude",
                         NULL,
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gairq_geo_init (GairqGeo *self)
{
  self->type = GAIRQ_GEO_TYPE_NONE;
  self->lat = NULL;
  self->lng = NULL;
}

/* --- Private Methods --- */
static gboolean
is_stringified_lat_lng (const gchar *value)
{
  gboolean ret = TRUE;

  for ( ; *value != '\0'; value++)
    {
      switch (*value)
        {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.':
          break;

        default:
          ret = FALSE;
          break;
        }
    }

  return ret;
}

/* --- Public APIs --- */
GairqGeo *
gairq_geo_new_with_ip (const gchar *access_token)
{
  g_return_val_if_fail (STR_VALIDATOR (access_token), NULL);

  return g_object_new (GAIRQ_TYPE_GEO,
                       "token", access_token,
                       "type", GAIRQ_GEO_TYPE_IP,
                       NULL);
}

GairqGeo *
gairq_geo_new_with_lat_lng (const gchar *access_token,
                            const gchar *lat,
                            const gchar *lng)
{
  g_return_val_if_fail (STR_VALIDATOR (access_token), NULL);
  g_return_val_if_fail (STR_VALIDATOR (lat), NULL);
  g_return_val_if_fail (STR_VALIDATOR (lng), NULL);
  g_return_val_if_fail (is_stringified_lat_lng (lat), NULL);
  g_return_val_if_fail (is_stringified_lat_lng (lng), NULL);

  return g_object_new (GAIRQ_TYPE_GEO,
                       "token", access_token,
                       "type", GAIRQ_GEO_TYPE_LATLNG,
                       "latitude", lat,
                       "longitude", lng,
                       NULL);
}

GairqAirObject *
gairq_geo_request_sync (GairqGeo  *self,
                        GError   **error)
{
  JsonNode *root = NULL;
  gpointer ret = NULL;

  g_return_val_if_fail (GAIRQ_IS_GEO (self), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  root = gairq_request_call_sync (GAIRQ_REQUEST (self), error);
  if (root)
    ret = gairq_request_default_deserialize (root, error);

  json_node_unref (root);

  return ret;
}

void
gairq_geo_request_async (GairqGeo            *self,
                         GCancellable        *cancellable,
                         GAsyncReadyCallback  callback,
                         gpointer             callback_data)
{
  g_return_if_fail (GAIRQ_IS_GEO (self));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  gairq_request_call_async (GAIRQ_REQUEST (self),
                            cancellable,
                            callback,
                            callback_data);
}

GairqAirObject *
gairq_geo_request_finish (GairqGeo      *self,
                          GAsyncResult  *res,
                          GError       **error)
{
  JsonNode *root;

  g_return_val_if_fail (GAIRQ_IS_GEO (self), NULL);
  g_return_val_if_fail (g_task_is_valid (res, self), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  root = g_task_propagate_pointer (G_TASK (res), error);
  if (root)
    return gairq_request_default_deserialize (root, error);

  return NULL;
}

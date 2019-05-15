/* gairq-city.c
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

#include "gairq-city.h"
#include "gairq-debug.h"
#include "gairq-utils.h"

#include <ctype.h>

#include <json-glib/json-glib.h>
#include <rest/rest-proxy.h>

struct _GairqCity
{
  GairqRequest    parent_instance;

  GairqCityType   type;
  gchar *         city;
};

/* Properties */
enum {
  PROP_0,
  PROP_TYPE,
  PROP_CITY,
  N_PROPERTIES
};

static GParamSpec* properties [N_PROPERTIES];

G_DEFINE_TYPE (GairqCity, gairq_city, GAIRQ_TYPE_REQUEST)

#define GAIRQ_CITY_ERROR (gairq_city_error_quark ())


/* --- GairqCityError --- */
static GQuark
gairq_city_error_quark (void)
{
  return g_quark_from_static_string ("gairq-city-error-quark");
}

/* --- GObject --- */
static void
gairq_city_finalize (GObject *object)
{
  GairqCity *self = GAIRQ_CITY (object);

  g_free (self->city);

  G_OBJECT_CLASS (gairq_city_parent_class)->finalize (object);
}

static void
gairq_city_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  GairqCity *self = GAIRQ_CITY (object);

  switch (prop_id)
    {
    case PROP_TYPE:
      self->type = g_value_get_uint (value);
      break;

    case PROP_CITY:
      g_free (self->city);
      self->city = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gairq_city_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  GairqCity *self = GAIRQ_CITY (object);

  switch (prop_id)
    {
    case PROP_TYPE:
      g_value_set_uint (value, self->type);
      break;

    case PROP_CITY:
      g_value_set_string (value, self->city);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static gboolean
gairq_city_request_set_functions (RestProxyCall  *proxy_call,
                                  gpointer        user_data,
                                  GError        **error)
{
  GairqCity *self = GAIRQ_CITY (user_data);
  gboolean ret = TRUE;
  gchar *functions = NULL;

  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  switch (self->type)
    {
    case GAIRQ_CITY_TYPE_NAME:
    case GAIRQ_CITY_TYPE_ID:
      functions = g_strdup_printf ("/feed/%s/", self->city);
      break;

    default:
      if (error)
        g_set_error (error, GAIRQ_CITY_ERROR, 0,
                     "Invalid city type code: %d", self->type);
      ret = FALSE;
    }

  if (ret)
    rest_proxy_call_set_function (proxy_call, functions);

  g_free (functions);

  return ret;
}

static gboolean
gairq_city_request_set_parameters (RestProxyCall  *proxy_call,
                                   gpointer        user_data,
                                   GError        **error)
{
  return TRUE;
}

static void
gairq_city_class_init (GairqCityClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GairqRequestClass *request_class = GAIRQ_REQUEST_CLASS (klass);

  object_class->finalize = gairq_city_finalize;
  object_class->set_property = gairq_city_set_property;
  object_class->get_property = gairq_city_get_property;

  request_class->set_functions = gairq_city_request_set_functions;
  request_class->set_parameters = gairq_city_request_set_parameters;

  properties [PROP_TYPE] =
    g_param_spec_uint ("type", "Type",
                       "A helper type for city property",
                       GAIRQ_CITY_TYPE_NONE,
                       N_GAIRQ_CITY_TYPES,
                       GAIRQ_CITY_TYPE_NONE,
                       G_PARAM_READWRITE);

  properties [PROP_CITY] =
    g_param_spec_string ("city", "City",
                         "A city name or id for air quality information",
                         NULL,
                         G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gairq_city_init (GairqCity *self)
{
  self->type = GAIRQ_CITY_TYPE_NONE;
  self->city = NULL;
}

/* --- Private APIs --- */
static GairqAirObject *
gairq_city_deserialize_json (JsonNode  *root,
                             GError   **error)
{
  JsonObject *object;
  JsonNode *status, *data;
  const gchar *status_msg;
  gpointer ret = NULL;

  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  object = json_node_get_object (root);
  status = json_object_get_member (object, "status");
  data = json_object_get_member (object, "data");

  status_msg = json_node_get_string (status);
  if (g_strcmp0 (status_msg, "ok") == 0)
    {
      ret = GAIRQ_AIR_OBJECT (json_gobject_deserialize (GAIRQ_TYPE_AIR_OBJECT, data));
    }
  else /* Set up new error message */
    {
      if (error)
        {
          const gchar *error_msg = json_node_get_string (data);

          g_set_error (error, GAIRQ_CITY_ERROR, 0,
                       "Error-Response: %s", error_msg);
        }
    }

  return ret;
}

/* --- Public APIs --- */
GairqCity *
gairq_city_new (const gchar   *access_token,
                GairqCityType  city_type,
                const gchar   *city)
{
  g_return_val_if_fail (STR_VALIDATOR (access_token), NULL);

  return g_object_new (GAIRQ_TYPE_CITY,
                       "token", access_token,
                       "type", city_type,
                       "city", city,
                       NULL);
}

GairqCity *
gairq_city_new_with_name (const gchar *access_token,
                          const gchar *city_name)
{
  const gchar *ch;

  g_return_val_if_fail (STR_VALIDATOR (access_token), NULL);
  g_return_val_if_fail (STR_VALIDATOR (city_name), NULL);

  for (ch = city_name; *ch != '\0'; ch++)
    {
      if (!isalpha (*ch))
        {
          gairq_debug ("%s is invalid for city_name parameter", city_name);
          return NULL;
        }
    }

  return g_object_new (GAIRQ_TYPE_CITY,
                       "token", access_token,
                       "type", GAIRQ_CITY_TYPE_NAME,
                       "city", city_name,
                       NULL);
}

GairqCity *
gairq_city_new_with_id (const gchar *access_token,
                        const gchar *city_id)
{
  const gchar *ch;
  gchar *real_id;
  gpointer ret;

  g_return_val_if_fail (STR_VALIDATOR (access_token), NULL);
  g_return_val_if_fail (STR_VALIDATOR (city_id), NULL);

  for (ch = city_id; *ch != '\0'; ch++)
    {
      if (!isdigit (*ch))
        {
          gairq_debug ("%s is invalid for city_id parameter", city_id);
          return NULL;
        }
    }

  real_id = g_strdup_printf ("@%s", city_id);

  ret = g_object_new (GAIRQ_TYPE_CITY,
                      "token", access_token,
                      "type", GAIRQ_CITY_TYPE_ID,
                      "city", real_id,
                      NULL);

  g_free (real_id);

  return ret;
}

GairqAirObject *
gairq_city_request_sync (GairqCity  *self,
                         GError    **error)
{
  JsonNode *root = NULL;
  gpointer ret = NULL;

  g_return_val_if_fail (GAIRQ_IS_CITY (self), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  root = gairq_request_call_sync (GAIRQ_REQUEST (self), error);
  if (root)
    ret = gairq_request_default_deserialize (root, error);

  json_node_unref (root);

  return ret;
}

void
gairq_city_request_async (GairqCity           *self,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             callback_data)
{
  g_return_if_fail (GAIRQ_IS_CITY (self));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  gairq_request_call_async (GAIRQ_REQUEST (self),
                            cancellable,
                            callback,
                            callback_data);
}

GairqAirObject *
gairq_city_request_finish (GairqCity     *self,
                           GAsyncResult  *res,
                           GError       **error)
{
  JsonNode *root;

  g_return_val_if_fail (GAIRQ_IS_CITY (self), NULL);
  g_return_val_if_fail (g_task_is_valid (res, self), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  root = g_task_propagate_pointer (G_TASK (res), error);
  if (root)
    return gairq_request_default_deserialize (root, error);

  return NULL;
}

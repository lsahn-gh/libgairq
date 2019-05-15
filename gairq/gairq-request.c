/* gairq-request.c
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

#include "gairq-request.h"
#include "gairq-request-priv.h"
#include "gairq-version.h"

typedef struct
{
  RestProxy * proxy;
  gchar *     token;
} GairqRequestPrivate;

/* Properties */
enum {
  PROP_0,
  PROP_TOKEN,
  N_PROPERTIES
};

static GParamSpec*  properties [N_PROPERTIES];

G_DEFINE_TYPE_WITH_PRIVATE (GairqRequest, gairq_request, G_TYPE_OBJECT)

#define GAIRQ_REQUEST_ERROR (gairq_request_error_quark ())

#define GET_PRIVATE(_obj) gairq_request_get_instance_private (GAIRQ_REQUEST (_obj))


/* --- GairqRequestError --- */
static GQuark
gairq_request_error_quark (void)
{
  return g_quark_from_static_string ("gairq-request-error-quark");
}

/* --- GObject --- */
static void
gairq_request_dispose (GObject *object)
{
  GairqRequestPrivate *priv = GET_PRIVATE (object);

  g_clear_object (&priv->proxy);

  G_OBJECT_CLASS (gairq_request_parent_class)->dispose (object);
}

static void
gairq_request_finalize (GObject *object)
{
  GairqRequestPrivate *priv = GET_PRIVATE (object);

  g_free (priv->token);

  G_OBJECT_CLASS (gairq_request_parent_class)->finalize (object);
}

static void
gairq_request_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GairqRequestPrivate *priv = GET_PRIVATE (object);

  switch (prop_id)
    {
    case PROP_TOKEN:
      g_free (priv->token);
      priv->token = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gairq_request_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GairqRequestPrivate *priv = GET_PRIVATE (object);

  switch (prop_id)
    {
    case PROP_TOKEN:
      g_value_set_string (value, priv->token);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static gboolean
gairq_request_set_functions (RestProxyCall  *proxy_call,
                             gpointer        user_data,
                             GError        **error)
{
  /* The class's children must override this method */
  g_assert_not_reached ();
}

static gboolean
gairq_request_set_parameters (RestProxyCall  *proxy_call,
                              gpointer        user_data,
                              GError        **error)
{
  /* The class's children must override this method */
  g_assert_not_reached ();
}

static void
gairq_request_class_init (GairqRequestClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gairq_request_dispose;
  object_class->finalize = gairq_request_finalize;
  object_class->set_property = gairq_request_set_property;
  object_class->get_property = gairq_request_get_property;

  klass->set_functions = gairq_request_set_functions;
  klass->set_parameters = gairq_request_set_parameters;

  /**
   * GairqRequest:token:
   *
   */
  properties [PROP_TOKEN] =
    g_param_spec_string ("token", "Token",
                         "An access token to request",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gairq_request_init (GairqRequest *self)
{
  GairqRequestPrivate *priv = GET_PRIVATE (self);

  priv->token = NULL;
  priv->proxy = rest_proxy_new (API_URL, FALSE);
  rest_proxy_set_user_agent (priv->proxy, "Gairq/" GAIRQ_VERSION_S);
}

/* --- Private Methods --- */
static void
gairq_request_call_io_thread (GTask        *task,
                              gpointer      source_object,
                              gpointer      task_data,
                              GCancellable *cancellable)
{
  GairqRequest *self = GAIRQ_REQUEST (source_object);
  GError *error = NULL;
  JsonNode *root;

  root = gairq_request_call_sync (self, &error);
  if (root)
    g_task_return_pointer (task, root, g_object_unref);
  else
    {
      if (error)
        g_task_return_error (task, error);

      /* If both, root and error are NULL, what should we do? */
    }
}

/* --- Public APIs --- */
GairqRequest *
gairq_request_new (const gchar *access_token)
{
  g_return_val_if_fail (access_token != NULL, NULL);

  return g_object_new (GAIRQ_TYPE_REQUEST,
                       "token", access_token,
                       NULL);
}

JsonNode *
gairq_request_call_sync (GairqRequest  *self,
                         GError       **error)
{
  GairqRequestPrivate *priv;
  RestProxyCall *proxy_call;
  JsonNode *ret = NULL;

  g_return_val_if_fail (GAIRQ_IS_REQUEST (self), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  priv = GET_PRIVATE (self);

  proxy_call = rest_proxy_new_call (priv->proxy);
  rest_proxy_call_set_method (proxy_call, "GET");

  /* Call overrided methods in which its children' own responsibility */
  if (!GAIRQ_REQUEST_GET_CLASS (self)->set_functions (proxy_call, self, error))
    goto out_error;
  if (!GAIRQ_REQUEST_GET_CLASS (self)->set_parameters (proxy_call, self, error))
    goto out_error;

  /* We don't care about token here, api server will
   * return an error in json way if it is invalid.
   */
  rest_proxy_call_add_param (proxy_call, "token", priv->token);

  if (rest_proxy_call_sync (proxy_call, error))
    {
      const gchar *payload;
      JsonParser *parser;

      payload = rest_proxy_call_get_payload (proxy_call);
      parser = json_parser_new ();

      if (json_parser_load_from_data (parser, payload, -1, error))
        ret = json_parser_steal_root (parser);

      g_object_unref (parser);
    }

out_error:
  g_object_unref (proxy_call);

  return ret;
}

void
gairq_request_call_async (GairqRequest        *self,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             callback_data)
{
  GTask *task;

  g_return_if_fail (GAIRQ_IS_REQUEST (self));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self,
                     cancellable,
                     callback,
                     callback_data);
  g_task_run_in_thread (task, gairq_request_call_io_thread);

  g_object_unref (task);
}

JsonNode *
gairq_request_call_finish (GairqRequest  *self,
                           GAsyncResult  *res,
                           GError       **error)
{
  g_return_val_if_fail (GAIRQ_IS_REQUEST (self), NULL);
  g_return_val_if_fail (g_task_is_valid (res, self), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  return g_task_propagate_pointer (G_TASK (res), error);
}

GairqAirObject *
gairq_request_default_deserialize (JsonNode  *root,
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

          g_set_error (error, GAIRQ_REQUEST_ERROR, 0,
                       "Error-Response: %s", error_msg);
        }
    }

  return ret;
}

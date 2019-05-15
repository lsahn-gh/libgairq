/* gairq-request.h
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

#ifndef GAIRQ_REQUEST_H
#define GAIRQ_REQUEST_H

#if !defined(GAIRQ_INSIDE) && !defined(GAIRQ_COMPILATION)
# error "Only <gairq/gairq.h> can be included directly."
#endif

#include <glib-object.h>
#include <gio/gio.h>
#include <json-glib/json-glib.h>
#include <rest/rest-proxy.h>

#include <gairq/gairq-air-object.h>

G_BEGIN_DECLS

#define GAIRQ_TYPE_REQUEST (gairq_request_get_type ())
G_DECLARE_DERIVABLE_TYPE (GairqRequest, gairq_request, GAIRQ, REQUEST, GObject)

struct _GairqRequestClass
{
  GObjectClass  parent_class;

  gboolean      (*set_functions)  (RestProxyCall  *proxy_call,
                                   gpointer        user_data,
                                   GError        **error);
  gboolean      (*set_parameters) (RestProxyCall  *proxy_call,
                                   gpointer        user_data,
                                   GError        **error);

  gpointer      _reserved1;
  gpointer      _reserved2;
  gpointer      _reserved3;
  gpointer      _reserved4;
};

GairqRequest *    gairq_request_new                 (const gchar *access_token);
JsonNode *        gairq_request_call_sync           (GairqRequest  *self,
                                                     GError       **error);
void              gairq_request_call_async          (GairqRequest        *self,
                                                     GCancellable        *cancellable,
                                                     GAsyncReadyCallback  callback,
                                                     gpointer             callback_data);
JsonNode *        gairq_request_call_finish         (GairqRequest  *self,
                                                     GAsyncResult  *res,
                                                     GError       **error);
GairqAirObject *  gairq_request_default_deserialize (JsonNode  *root,
                                                     GError   **error);

G_END_DECLS

#endif

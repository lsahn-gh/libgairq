/* gairq.h
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

#ifndef GAIRQ_H
#define GAIRQ_H

#include <glib.h>

G_BEGIN_DECLS

#define GAIRQ_INSIDE
# include <gairq/gairq-air-object.h>
# include <gairq/gairq-city.h>
# include <gairq/gairq-geo.h>
# include <gairq/gairq-request.h>
# include <gairq/gairq-version.h>
#undef GAIRQ_INSIDE

G_END_DECLS

#endif

/**
 *
 * HiveeyesTelemetry - a flexible telemetry client for beehive monitoring
 *
 *
 *  Copyright (C) 2015-2016  Andreas Motl <andreas.motl@elmyra.de>
 *  Copyright (C) 2015-2016  Richard Pobering <einsiedlerkrebs@ginnungagap.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  See also "LICENSE.txt" in the root folder of
 *  this repository or distribution package.
 *
 *
 * HiveeyesTelemetry provides the infrastructure components for
 * supporting different communication paths. We talk GPRS and RFM69.
 *
**/
#include "HiveeyesTelemetry.h"

using namespace Hiveeyes;

/**
 *
 * A NodeAddress with realm set to "hiveeyes" to
 * participate in collaborative data collection.
 *
**/
HiveeyesNodeAddress::HiveeyesNodeAddress(const char *network, const char *gateway, const char *node)
    : NodeAddress("hiveeyes", network, gateway, node)
{}

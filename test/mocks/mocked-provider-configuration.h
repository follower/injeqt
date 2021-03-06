/*
 * %injeqt copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %injeqt copyright end%
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "provider.h"

#include "provider-configuration.h"

using namespace injeqt::internal;
using namespace injeqt::v1;

class mocked_provider_configuration : public provider_configuration
{

public:
	mocked_provider_configuration() {}
	virtual ~mocked_provider_configuration() {}

    virtual std::vector<type> types() const override { return {}; }
	virtual std::unique_ptr<provider> create_provider(const types_by_name &) const override { return nullptr; }

};

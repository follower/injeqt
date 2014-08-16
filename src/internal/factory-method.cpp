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

#include "factory-method.h"

#include "extract-interfaces.h"

namespace injeqt { namespace internal {

factory_method::factory_method(QMetaMethod meta_method) :
	_object_type{meta_method.enclosingMetaObject()},
	_result_type{QMetaType::metaObjectForType(meta_method.returnType())},
	_meta_method{std::move(meta_method)}
{
	if (meta_method.methodType() != QMetaMethod::Method &&
		meta_method.methodType() != QMetaMethod::Slot)
		throw invalid_factory_method_exception{};
	if (meta_method.parameterCount() != 0)
		throw invalid_factory_method_exception{};

	try
	{
		validate(_object_type);
		validate(_result_type);
	}
	catch (invalid_type_exception &e)
	{
		throw invalid_factory_method_exception{};
	}
}

const type & factory_method::object_type() const
{
	return _object_type;
}

const type & factory_method::result_type() const
{
	return _result_type;
}

std::unique_ptr<QObject> factory_method::invoke(QObject *on) const
{
	QObject *result = nullptr;
	_meta_method.invoke(on, QReturnArgument<QObject *>((_result_type.name() + "*").c_str(), result)); // TODO: check for false result
	return std::unique_ptr<QObject>{result};
}

bool operator == (const factory_method &x, const factory_method &y)
{
	if (x.object_type() != y.object_type())
		return false;

	if (x.result_type() != y.result_type())
		return false;

	return true;
}

bool operator != (const factory_method &x, const factory_method &y)
{
	return !(x == y);
}

factory_method make_factory_method(const type &t, const type &f)
{
	auto meta_object = f.meta_object();
	auto method_count = meta_object->methodCount();
	auto factory_methods = std::vector<factory_method>{};

	for (decltype(method_count) i = 0; i < method_count; i++)
	{
		auto method = meta_object->method(i);
		if (method.parameterCount() != 0)
			continue;
		auto return_type_meta_object = QMetaType::metaObjectForType(method.returnType());
		if (!return_type_meta_object)
			continue;
		auto return_type = type{return_type_meta_object};
		auto interfaces = extract_interfaces(return_type);
		if (interfaces.contains(t))
			factory_methods.emplace_back(method);
	}

	if (factory_methods.empty())
		throw no_factory_method_exception{t.name()};
	else if (factory_methods.size() > 1)
		throw non_unique_factory_exception{t.name()};
	else
		return factory_methods.front();
}

}}

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

#include "expect.h"
#include "utils.h"

#include <injeqt/exception/dependency-duplicated.h>
#include <injeqt/exception/dependency-on-self.h>
#include <injeqt/exception/dependency-on-subtype.h>
#include <injeqt/exception/dependency-on-supertype.h>
#include <injeqt/exception/invalid-setter.h>

#include "internal/dependencies.h"

#include <QtTest/QtTest>

using namespace injeqt::internal;
using namespace injeqt::v1;

class injectable_type1 : public QObject
{
	Q_OBJECT
};

class sub_injectable_type1a : public injectable_type1
{
	Q_OBJECT
};

class sub_injectable_type1b : public injectable_type1
{
	Q_OBJECT
};

class injectable_type2 : public QObject
{
	Q_OBJECT
};

class injectable_type3 : public QObject
{
	Q_OBJECT
};

class valid_injected_type : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE INJEQT_SET void setter_1(injectable_type1 *) {}

public slots:
	INJEQT_SET void setter_2(injectable_type2 *) {}
	void no_injeqt_setter_1(injectable_type1 *) {}
	void no_injeqt_setter_2(injectable_type2 *) {}
	void no_injeqt_setter_3(int) {}

};

class inheriting_valid_injected_type : public valid_injected_type
{
	Q_OBJECT

public slots:
	INJEQT_SET void setter_3(injectable_type3 *) {}

};

class valid_injected_type_with_common_superclass : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE INJEQT_SET void setter_1(sub_injectable_type1a *) {}

public slots:
	INJEQT_SET void setter_2(sub_injectable_type1b *) {}

};

class duplicate_dependency_injected_type : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE INJEQT_SET void setter_1(injectable_type1 *) {}

public slots:
	INJEQT_SET void setter_2(injectable_type1 *) {}

};

class injected_type_with_superclass : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE INJEQT_SET void setter_1(injectable_type1 *) {}

public slots:
	INJEQT_SET void setter_2(sub_injectable_type1a *) {}

};

class injected_type_with_superclass_inverted : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE INJEQT_SET void setter_2(sub_injectable_type1a *) {}

public slots:
	INJEQT_SET void setter_1(injectable_type1 *) {}

};

class invalid_injected_type_depends_on_self : public QObject
{
	Q_OBJECT

public slots:
	INJEQT_SET void setter_1(invalid_injected_type_depends_on_self *) {}

};

class invalid_injected_type_depends_on_subtype_subtype;

class invalid_injected_type_depends_on_subtype : public QObject
{
	Q_OBJECT

public slots:
	INJEQT_SET void setter_1(invalid_injected_type_depends_on_subtype_subtype *) {}

};

class invalid_injected_type_depends_on_subtype_subtype : public invalid_injected_type_depends_on_subtype
{
	Q_OBJECT
};

class invalid_injected_type_depends_on_supertype_supertype : public QObject
{
	Q_OBJECT
};

class invalid_injected_type_depends_on_supertype : public invalid_injected_type_depends_on_supertype_supertype
{
	Q_OBJECT

public slots:
	INJEQT_SET void setter_1(invalid_injected_type_depends_on_supertype_supertype *) {}

};

class invalid_setter_empty_type : public QObject
{
	Q_OBJECT

public slots:
	INJEQT_SET void setter_int(int) {};

};

class invalid_setter_qobject_type : public QObject
{
	Q_OBJECT

public slots:
	INJEQT_SET void setter_qobject(QObject *) {};

};

class invalid_setter_no_paremeters : public QObject
{
	Q_OBJECT

public slots:
	INJEQT_SET void setter_no_paremeters() {};

};

class invalid_setter_two_paremters : public QObject
{
	Q_OBJECT

public slots:
	INJEQT_SET void setter_two_parameters(injectable_type1 *, injectable_type2 *) {};

};

class invalid_setter_is_signal: public QObject
{
	Q_OBJECT

signals:
	INJEQT_SET void setter_two_parameters(injectable_type1 *);

};

class dependencies_test : public QObject
{
	Q_OBJECT

public:
    dependencies_test();

private slots:
	void should_find_no_dependnecies_in_qobject();
	void should_find_all_valid_dependencies();
	void should_find_all_valid_dependencies_in_hierarchy();
	void should_find_dependencies_with_common_superclass();
	void should_accept_duplicate_dependency();
	void should_accept_superclass_dependency();
	void should_accept_superclass_inverted_dependency();
	void should_fail_when_depends_on_self();
	void should_fail_when_depends_on_subtype();
	void should_fail_when_depends_on_supertype();
	void should_throw_when_setter_with_empty_type();
	void should_throw_when_setter_with_qobject_type();
	void should_throw_when_setter_with_no_parameters();
	void should_throw_when_setter_with_two_parameters();
	void should_throw_when_setter_is_signal();

private:
	types_by_name known_types;

	void verify_dependency(dependencies list, const dependency &check);

};

dependencies_test::dependencies_test()
{
	known_types = types_by_name{std::vector<type>{
		make_type<injectable_type1>(),
		make_type<sub_injectable_type1a>(),
		make_type<sub_injectable_type1b>(),
		make_type<injectable_type2>(),
		make_type<injectable_type3>(),
		make_type<valid_injected_type>(),
		make_type<inheriting_valid_injected_type>(),
		make_type<valid_injected_type_with_common_superclass>(),
		make_type<duplicate_dependency_injected_type>(),
		make_type<injected_type_with_superclass>(),
		make_type<injected_type_with_superclass_inverted>(),
		make_type<invalid_injected_type_depends_on_self>(),
		make_type<invalid_injected_type_depends_on_subtype_subtype>(),
		make_type<invalid_injected_type_depends_on_subtype>(),
		make_type<invalid_injected_type_depends_on_supertype_supertype>(),
		make_type<invalid_injected_type_depends_on_supertype>(),
		make_type<invalid_setter_empty_type>(),
		make_type<invalid_setter_qobject_type>(),
		make_type<invalid_setter_no_paremeters>(),
		make_type<invalid_setter_two_paremters>()
	}};
}

void dependencies_test::verify_dependency(dependencies list, const dependency &check)
{
	auto iterator = std::find(begin(list), end(list), check);
	QVERIFY(iterator != end(list));
}

void dependencies_test::should_find_no_dependnecies_in_qobject()
{
	auto dependencies = extract_dependencies(known_types, make_type<QObject>());
	QCOMPARE(dependencies.size(), size_t{0});
}

void dependencies_test::should_find_all_valid_dependencies()
{
	auto dependencies = extract_dependencies(known_types, make_type<valid_injected_type>());
	QCOMPARE(dependencies.size(), size_t{2});
	verify_dependency(dependencies, dependency{make_test_setter_method<valid_injected_type, injectable_type1>("setter_1(injectable_type1*)")});
	verify_dependency(dependencies, dependency{make_test_setter_method<valid_injected_type, injectable_type2>("setter_2(injectable_type2*)")});
}

void dependencies_test::should_find_all_valid_dependencies_in_hierarchy()
{
	auto dependencies = extract_dependencies(known_types, make_type<inheriting_valid_injected_type>());
	QCOMPARE(dependencies.size(), size_t{3});
	verify_dependency(dependencies, dependency{make_test_setter_method<valid_injected_type, injectable_type1>("setter_1(injectable_type1*)")});
	verify_dependency(dependencies, dependency{make_test_setter_method<valid_injected_type, injectable_type2>("setter_2(injectable_type2*)")});
	verify_dependency(dependencies, dependency{make_test_setter_method<inheriting_valid_injected_type, injectable_type3>("setter_3(injectable_type3*)")});
}

void dependencies_test::should_find_dependencies_with_common_superclass()
{
	auto dependencies = extract_dependencies(known_types, make_type<valid_injected_type_with_common_superclass>());
	QCOMPARE(dependencies.size(), size_t{2});
	verify_dependency(dependencies, dependency{make_test_setter_method<valid_injected_type_with_common_superclass, sub_injectable_type1a>("setter_1(sub_injectable_type1a*)")});
	verify_dependency(dependencies, dependency{make_test_setter_method<valid_injected_type_with_common_superclass, sub_injectable_type1b>("setter_2(sub_injectable_type1b*)")});
}

void dependencies_test::should_accept_duplicate_dependency()
{
	auto dependencies = extract_dependencies(known_types, make_type<duplicate_dependency_injected_type>());
	QCOMPARE(dependencies.size(), size_t{2});
	verify_dependency(dependencies, dependency{make_test_setter_method<duplicate_dependency_injected_type, injectable_type1>("setter_1(injectable_type1*)")});
	verify_dependency(dependencies, dependency{make_test_setter_method<duplicate_dependency_injected_type, injectable_type1>("setter_2(injectable_type1*)")});
}

void dependencies_test::should_accept_superclass_dependency()
{
	auto dependencies = extract_dependencies(known_types, make_type<injected_type_with_superclass>());
	QCOMPARE(dependencies.size(), size_t{2});
	verify_dependency(dependencies, dependency{make_test_setter_method<injected_type_with_superclass, injectable_type1>("setter_1(injectable_type1*)")});
	verify_dependency(dependencies, dependency{make_test_setter_method<injected_type_with_superclass, sub_injectable_type1a>("setter_2(sub_injectable_type1a*)")});
}

void dependencies_test::should_accept_superclass_inverted_dependency()
{
	auto dependencies = extract_dependencies(known_types, make_type<injected_type_with_superclass_inverted>());
	QCOMPARE(dependencies.size(), size_t{2});
	verify_dependency(dependencies, dependency{make_test_setter_method<injected_type_with_superclass_inverted, sub_injectable_type1a>("setter_2(sub_injectable_type1a*)")});
	verify_dependency(dependencies, dependency{make_test_setter_method<injected_type_with_superclass_inverted, injectable_type1>("setter_1(injectable_type1*)")});
}

void dependencies_test::should_fail_when_depends_on_self()
{
	expect<exception::dependency_on_self>({}, [&]{
		auto dependencies = extract_dependencies(known_types, make_type<invalid_injected_type_depends_on_self>());
	});
}

void dependencies_test::should_fail_when_depends_on_subtype()
{
	expect<exception::dependency_on_subtype>({}, [&]{
		auto dependencies = extract_dependencies(known_types, make_type<invalid_injected_type_depends_on_subtype>());
	});
}

void dependencies_test::should_fail_when_depends_on_supertype()
{
	expect<exception::dependency_on_supertype>({}, [&]{
		auto dependencies = extract_dependencies(known_types, make_type<invalid_injected_type_depends_on_supertype>());
	});
}

void dependencies_test::should_throw_when_setter_with_empty_type()
{
	expect<exception::invalid_setter>({"setter_int"}, [&]{
		auto dependencies = extract_dependencies(known_types, make_type<invalid_setter_empty_type>());
	});
}

void dependencies_test::should_throw_when_setter_with_qobject_type()
{
	expect<exception::invalid_setter>({"setter_qobject"}, [&]{
		auto dependencies = extract_dependencies(known_types, make_type<invalid_setter_qobject_type>());
	});
}

void dependencies_test::should_throw_when_setter_with_no_parameters()
{
	expect<exception::invalid_setter>({"invalid parameter count"}, [&]{
		auto dependencies = extract_dependencies(known_types, make_type<invalid_setter_no_paremeters>());
	});
}

void dependencies_test::should_throw_when_setter_with_two_parameters()
{
	expect<exception::invalid_setter>({"invalid parameter count"}, [&]{
		auto dependencies = extract_dependencies(known_types, make_type<invalid_setter_two_paremters>());
	});
}

void dependencies_test::should_throw_when_setter_is_signal()
{
	expect<exception::invalid_setter>({"setter is signal"}, [&]{
		auto dependencies = extract_dependencies(known_types, make_type<invalid_setter_is_signal>());
	});
}

QTEST_APPLESS_MAIN(dependencies_test);

#include "dependencies-test.moc"

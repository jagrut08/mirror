# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
include(Example.cmake)

add_subdirectory(testdecl)

mirror_add_simple_example(amount_of_foo)
mirror_add_simple_example(applicable_ops)
mirror_add_simple_example(chai_on_mirror)
mirror_add_simple_example(ctre_integer_concept)
mirror_add_simple_example(expression)
mirror_add_simple_example(filter)
mirror_add_simple_example(fake_rpc)
mirror_add_simple_example(find_if)
mirror_add_simple_example(from_rapidjson)
mirror_add_simple_example(gen_puml_diagram)
mirror_add_simple_example(good_example)
mirror_add_simple_example(has_padding)
mirror_add_simple_example(hello_world)
mirror_add_simple_example(invoke)
mirror_add_simple_example(next_weekday)
mirror_add_simple_example(print_enumerators)
mirror_add_simple_example(print_full_name)
mirror_add_simple_example(print_months)
mirror_add_simple_example(print_op_results)
mirror_add_simple_example(print_struct)
mirror_add_simple_example(print_traits)
mirror_add_simple_example(repeat_message_args)
mirror_add_simple_example(repeat_message_json)
mirror_add_simple_example(row_polymorphism)
mirror_add_simple_example(simple_json)
mirror_add_simple_example(to_rapidjson)

mirror_add_simple_example(puml_class_diagram)
target_link_libraries(mirror-puml_class_diagram PRIVATE mirror-testdecl)


add_subdirectory(factory)

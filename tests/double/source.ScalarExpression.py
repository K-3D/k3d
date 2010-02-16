#python

import testing

setup = testing.setup_scalar_source_test("ScalarExpression")
setup.source.expression = "cos(pi)"

testing.require_scalar_value(setup.source.output, -1.0)


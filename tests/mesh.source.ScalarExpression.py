#python

import testing

setup = testing.setup_scalar_source_test("ScalarExpression")
setup.source.expression = "cos(pi)"

testing.scalar_comparison(setup.source.output, -1.0)


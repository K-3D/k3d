# This code is a heavily-modified version of the Pythoni 2.6 "rlcompleter" module,
# altered for use with the K-3D Python interactive shell.  

def get_class_members(klass):
	ret = dir(klass)
	if hasattr(klass,'__bases__'):
		for base in klass.__bases__:
			ret = ret + get_class_members(base)
	return ret

def attribute_matches(text, namespace):
	import re
	m = re.match(r"(\w+(\.\w+)*)\.(\w*)", text)
	if not m:
		return []
	expr, attr = m.group(1, 3)
	try:
		thisobject = eval(expr, namespace)
	except Exception:
		return []

	# get the content of the object, except __builtins__
	words = dir(thisobject)
	if "__builtins__" in words:
		words.remove("__builtins__")

	if hasattr(thisobject, '__class__'):
		words.append('__class__')
		words.extend(get_class_members(thisobject.__class__))
		matches = []
		n = len(attr)
		for word in words:
			if word[:n] == attr and hasattr(thisobject, word):
				val = getattr(thisobject, word)
				word = callable_postfix(val, "%s.%s" % (expr, word))
				matches.append(word)
		return matches

def callable_postfix(val, word):
	if hasattr(val, '__call__'):
		word = word + "("
	return word

def global_matches(text, namespace):
	matches = []
	n = len(text)
	import keyword
	for word in keyword.kwlist:
		if word[:n] == text:
			matches.append(word)
	import __builtin__
	for nspace in [__builtin__.__dict__, namespace]:
		for word, val in nspace.items():
			if word[:n] == text and word != "__builtins__":
				matches.append(callable_postfix(val, word))
	return matches

def complete(text, namespace = locals()):
	matches = []

	if "." in text:
		return attribute_matches(text, namespace)
	else:
		return global_matches(text, namespace)

	return matches

if __name__ == "__main__":
	context["matches"] = complete(context.command)

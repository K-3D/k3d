from optparse import OptionParser
import re

def main():
	parser = OptionParser()
	#parser.add_option("--skip-rows", type="int", dest="skip_rows", default="0", help="Number of lines/rows to skip when using --lines or --csv.	Default: %default")
	(options, arguments) = parser.parse_args()

	listing_markup = "------------------------------------------------------"

	for path in arguments:
		tag_listing = False
		block_listing = False
		for markup in open(path, "r"):
			if re.search("<pre>", markup):
				tag_listing = True
				markup = re.sub("<pre>", listing_markup, markup)
			elif re.search("</pre>", markup):
				tag_listing = False
				markup = re.sub("</pre>", listing_markup, markup)
			elif markup.startswith(" ") and not block_listing and not tag_listing:
				print listing_markup
				block_listing = True
				markup = markup[1:]
			elif not markup.startswith(" ") and block_listing:
				print listing_markup
				block_listing = False

			if not tag_listing and not block_listing:
				# Convert varying numbers of single-quotes to Asciidoc "emphasis"
				markup = re.sub("('{2,5})([^']+('[^']+)*)\\1", "'\\2'", markup)
				# Markup double-quotes
				markup = re.sub("\"([^\"]*)\"", "``\\1''", markup)
				# Eliminate article categories
				markup = re.sub("\\[\\[Category:[^\]]*\\]\\]", "", markup)
				# Convert wiki links to Asciidoc links
				markup = re.sub("\\[\\[([^\]]*)\\]\\]", "<<\\1,\\1>>", markup)

			print markup,
		
if __name__ == "__main__":
	main()

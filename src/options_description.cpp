//<boost / program_options / options_description.hpp>

class options_description {
public:
	// construct/copy/destruct
	options_description(unsigned = m_default_line_length);
	options_description(const std::string &, unsigned = m_default_line_length);

	// public member functions
	void add(shared_ptr< option_description >);
	options_description & add(const options_description &);
	options_description_easy_init add_options();
	const option_description & find(const std::string &, bool) const;
	const option_description * find_nothrow(const std::string &, bool) const;
	const std::vector< shared_ptr< option_description > > & options() const;
	void print(std::ostream &) const;
	static const unsigned m_default_line_length;
};
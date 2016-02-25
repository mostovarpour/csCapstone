
class option_description {
public:

	enum match_result { no_match, full_match, approximate_match };
	// construct/copy/destruct
	option_description();
	option_description(const char *, const value_semantic *);
	option_description(const char *, const value_semantic *, const char *);
	~option_description();

	// public member functions
	match_result match(const std::string &, bool) const;
	const std::string & key(const std::string &) const;
	const std::string & long_name() const;
	const std::string & description() const;
	shared_ptr< const value_semantic > semantic() const;
	std::string format_name() const;
	std::string format_parameter() const;

	// private member functions
	option_description & set_name(const char *);
};
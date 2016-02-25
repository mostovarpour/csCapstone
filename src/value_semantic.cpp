class value_semantic {
public:
	// construct/copy/destruct
	~value_semantic();

	// public member functions
	std::string name() const;
	unsigned min_tokens() const;
	unsigned max_tokens() const;
	bool is_composing() const;
	void parse(boost::any &, const std::vector< std::string > &, bool) const;
	bool apply_default(boost::any &) const;
	void notify(const boost::any &) const;
};
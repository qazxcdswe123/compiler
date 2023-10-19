//
// Created by yn on 14/10/2023.
//

#ifndef COMPILER_RE_H
#define COMPILER_RE_H

#include <string>

class Regexp {
private:
    std::string re_;
    std::string post_re_;

    bool re2post();

public:
    explicit Regexp(std::string re) {
        this->re_ = std::move(re);
        normalize_char_class(this->re_);
        if (!re2post()) {
            throw std::runtime_error("Invalid regular expression: " + this->re_);
        }
    }

    void normalize_char_class(std::string_view re);

    std::string_view get_re() const {
        return re_;
    }

    std::string_view get_post_re() const {
        return post_re_;
    }
};


#endif //COMPILER_RE_H

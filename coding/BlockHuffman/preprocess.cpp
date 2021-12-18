#include "preprocess.h"

char ADDITIONAL_SYMBOL = '$';

void preprocess_message(std::string &message) {
    message += ADDITIONAL_SYMBOL;
}

void rollback_preprocess(std::string &message) {
    message.pop_back();
}

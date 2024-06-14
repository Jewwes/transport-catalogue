#include "json_builder.h"
#include <variant>

using namespace std::literals;

namespace json {

Builder::Builder()
    : root_()
    , nodes_stack_{&root_}
{}
    
void Builder::Nodes_stack_empty_(){
    if (nodes_stack_.empty()) {
        throw std::logic_error("Attempt to change finalized JSON"s);
    }
}
    
DictKeyContext Builder::Key(std::string key) {
    Nodes_stack_empty_();
    Node* top = nodes_stack_.back();
    if (!top->IsDict()) {
        throw std::logic_error("Key() outside a dict"s);
    }
    nodes_stack_.emplace_back(&const_cast<Dict&>(nodes_stack_.back()->AsDict())[std::move(key)]);
    return *this;
}

Builder& Builder::Value(Node::Value value) {
    Nodes_stack_empty_();
    Node* top = nodes_stack_.back();
    if (top->IsArray()) {
        const_cast<Array&>(nodes_stack_.back()->AsArray()).emplace_back(std::move(value));
    } else {
        top->GetValue() = std::move(value);
        nodes_stack_.pop_back();
    }
    return *this;
}

DictItemContext Builder::StartDict() {
    Nodes_stack_empty_();
    Node* top = nodes_stack_.back();
    if (top->IsArray()) {
        Node& node = const_cast<Array&>(nodes_stack_.back()->AsArray()).emplace_back(std::move(Dict()));
        nodes_stack_.push_back(&node);
    } else {
        top->GetValue() = std::move(Dict());
    }
    return *this;
}

ArrayItemContext Builder::StartArray() {
    Nodes_stack_empty_();
        Node* top = nodes_stack_.back();
    if (top->IsArray()) {
        Node& node = const_cast<Array&>(nodes_stack_.back()->AsArray()).emplace_back(std::move(Array()));
        nodes_stack_.push_back(&node);
    } else {
        top->GetValue() = std::move(Array());
    }
    return *this;
}

Builder& Builder::EndDict() {
    Nodes_stack_empty_();
    Node* top = nodes_stack_.back();
    if (!top->IsDict()) {
        throw std::logic_error("EndDict() outside a dict"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder& Builder::EndArray() {
    Nodes_stack_empty_();
    Node* top = nodes_stack_.back();
    if (!top->IsArray()) {
        throw std::logic_error("EndArray() outside an array"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Node Builder::Build() {
    if (!nodes_stack_.empty()) {
        throw std::logic_error("Attempt to build JSON which isn't finalized"s);
    }
    return std::move(root_);
}
    

DictKeyContext DictItemContext::Key(std::string key) {
    return builder_.Key(key);
}

Builder& DictItemContext::EndDict() {
    return builder_.EndDict();
}

ArrayItemContext ArrayItemContext::Value(Node::Value value) {
    return ArrayItemContext(builder_.Value(value));
}

DictItemContext ArrayItemContext::StartDict() {
    return builder_.StartDict();
}

ArrayItemContext ArrayItemContext::StartArray() {
    return builder_.StartArray();
}

Builder& ArrayItemContext::EndArray() {
    return builder_.EndArray();
}

DictItemContext DictKeyContext::Value(Node::Value value) {
    return DictItemContext(builder_.Value(value));
}

ArrayItemContext DictKeyContext::StartArray() {
    return builder_.StartArray();
}

DictItemContext DictKeyContext::StartDict() {
    return builder_.StartDict();
}
    
}
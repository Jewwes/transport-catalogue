#pragma once

#include <string>
#include <vector>
#include "json.h"

namespace json {

    class BaseContext;
    class DictItemContext;
    class DictKeyContext;
    class ArrayItemContext;
    
    
class Builder {
public:
    Builder();
    DictKeyContext Key(std::string key);
    Builder& Value(Node::Value value);
    DictItemContext StartDict();
    Builder& EndDict();
    ArrayItemContext StartArray();
    Builder& EndArray();
    Node Build();

private:
    Node root_;
    std::vector<Node*> nodes_stack_;
    void Nodes_stack_empty_();
};

class BaseContext {
public:
    BaseContext(Builder& builder) : builder_(builder)
    {}
    DictKeyContext Key(std::string);
    Builder& Value(Node::Value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndDict();
    Builder& EndArray();
    json::Node Build();
protected:
    Builder& builder_;
};
    
class DictItemContext : public BaseContext{
public:
    DictItemContext(Builder& builder) 
        : BaseContext(builder)
    {}
    DictKeyContext Key(std::string key);
    Builder& EndDict();
    Builder& Value(Node::Value) = delete;
    DictItemContext StartDict() = delete;
    ArrayItemContext StartArray() = delete;
    Builder& EndArray() = delete;
    json::Node Build() = delete;
};

class ArrayItemContext : public BaseContext {
public:
    ArrayItemContext(Builder& builder)
        : BaseContext(builder)
    {}

    ArrayItemContext Value(Node::Value value);
    DictItemContext StartDict();
    Builder& EndArray();
    ArrayItemContext StartArray();
    DictKeyContext Key(std::string key) = delete;
    Builder& EndDict() = delete;
    json::Node Build() = delete;    
};

class DictKeyContext : public BaseContext {
public:
    DictKeyContext(Builder& builder)
        : BaseContext(builder)
    {}
    DictItemContext Value(Node::Value value);
    ArrayItemContext StartArray();
    DictItemContext StartDict();
    DictKeyContext Key(std::string key) = delete;
    Builder& EndArray() = delete;
    Builder& EndDict() = delete;
    json::Node Build() = delete;
};
    
} 

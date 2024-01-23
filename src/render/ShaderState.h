//
// Copyright (c) 2021 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "Shader.h"

namespace baseline {

    class ShaderState {
    public:
        template<typename T>
        void set(const std::string& uniform, T value) {
            for (auto& state : states) {
                if (state && state->name == uniform) {
                    state = std::static_pointer_cast<Base>(std::make_shared<Value<T>>(uniform, value));
                    return;
                }
            }
            states.push_back(std::static_pointer_cast<Base>(std::make_shared<Value<T>>(uniform, value)));
        }

        template<typename T>
        void set(const std::string& uniform, T *values, int count) {
            for (auto& state : states) {
                if (state && state->name == uniform) {
                    state = std::static_pointer_cast<Base>(std::make_shared<Values<T>>(uniform, values, count));
                    return;
                }
            }
            states.push_back(std::static_pointer_cast<Base>(std::make_shared<Values<T>>(uniform, values, count)));
        }

        void apply(Shader *shader) {
            for (auto& state : states) {
                if (state) {
                    state->apply(shader);
                }
            }
        }

    private:
        class Base {
        public:
            std::string name;
            virtual void apply(Shader* shader) = 0;
            virtual void apply(ShaderState* shaderState) = 0;

            virtual void* getData() = 0;
        };

        template<typename T>
        class Value : public Base {
        public:
            T value;

            Value(const std::string& name, T value) {
                this->name = name;
                this->value = value;
            }
            
            void apply(Shader* shader) override {
                shader->set(name, value);
            }

            void apply(ShaderState* shaderState) override {
                shaderState->set(name, value);
            }

            virtual void* getData() {
                return &value;
            }
        };

        template<typename T>
        class Values : public Base {
        public:
            std::vector<T> values;

            Values(const std::string& name, T* values, int count) {
                this->name = name;
                this->values.insert(this->values.begin(), values, values + count);
            }

            void apply(Shader* shader) override {
                shader->set(name, values.data(), (int)values.size());
            }

            void apply(ShaderState* shaderState) override {
                shaderState->set(name, values.data(), (int)values.size());
            }

            virtual void* getData() {
                return &values;
            }
        };

        std::vector<std::shared_ptr<Base>> states;

    public:
        const std::vector<std::shared_ptr<Base>>& getValues() { return states; }
    };

}
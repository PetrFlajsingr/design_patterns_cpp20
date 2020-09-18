//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_VISITOR_H
#define DESIGN_PATTERNS_VISITOR_H
namespace pf {
template<typename... Ts>
struct visitor : Ts... { using Ts::operator()...; };
template<class... Ts>
visitor(Ts...) -> visitor<Ts...>;
}// namespace pf
#endif//DESIGN_PATTERNS_VISITOR_H

#pragma once

#include <string>
#include "context.h"
#include "func_invocation.h"

namespace scidf
{
    static std::string resolve(const std::string& stuff, const context_t& context)
    {
        bool debug = (stuff == "@.invoke_var(j)");
        
        if (stuff.length()==0) return stuff;
        std::size_t varpos  = stuff.find_first_of(context.get_syms().invoke_var);
        std::size_t funcpos = stuff.find_first_of(context.get_syms().invoke_func);
        if ((varpos == std::string::npos) && (funcpos == std::string::npos)) return stuff;
        
        if (varpos != std::string::npos)
        {
            if (varpos == stuff.length()-1) throw sdf_exception("variable invocation without variable name");
            std::size_t opos = stuff.find_first_of(context.get_syms().open_arg, varpos);
            if (opos == std::string::npos || opos >= stuff.length()-1) throw sdf_exception("ill-formatted variable invocation");
            std::string fname = stuff.substr(varpos + 1, opos - varpos - 1);
            if (fname.length() == 0)
            {
                const std::string new_content = str::overwrite_insert_at(stuff, varpos, context.get_syms().invoke_func + context.get_syms().builtin_invoke);
                return resolve(new_content, context);
            }
            else
            {
                std::string new_content = stuff.substr(0, varpos);
                new_content += (context.get_syms().invoke_func + context.get_syms().builtin_invoke + context.get_syms().open_arg);
                new_content += fname + context.get_syms().argument_separator;
                new_content += stuff.substr(opos+1, stuff.length() - opos - 1);
                return resolve(new_content, context);
            }
        }
        
        if (funcpos >= stuff.length()-1) throw sdf_exception("ill-formatted invocation");
        std::size_t fstart = funcpos;
        std::size_t fend   = fstart;
        int level = 0;
        bool found_content = false;
        while(!(level == 0 && found_content))
        {
            if (fend++ >= stuff.length()-1) throw sdf_exception("detected runaway argument");
            if (stuff[fend] == context.get_syms().open_arg)  level++;
            if (stuff[fend] == context.get_syms().close_arg) level--;
            found_content = found_content || (level > 0);
            if (level < 0) throw sdf_exception("unmatched argument braces");
        }
        fend++;
        std::string finv = stuff.substr(fstart, fend - fstart);
        auto [func_name, func_args] = parse_func_invokation(finv, context);
        if (func_name == context.get_syms().builtin_invoke)
        {
            if (func_args.size() == 1)
            {
                const auto& expression = context.get_expression(func_args[0]);
                std::string pre = stuff.substr(0, fstart);
                std::string mid = resolve(expression.content, context);
                std::string end = resolve(stuff.substr(fend, stuff.length() - fend), context);
                return pre + mid + end;
            }
            else if (func_args.size() > 1)
            {
                const auto& expression = context.get_expression(func_args[0]);
                if (func_args.size() != expression.parameter_names.size() + 1)
                    throw sdf_exception("incorrect number of arguments, expecting "
                        + std::to_string(expression.parameter_names.size())
                        + ", got " + std::to_string(func_args.size() - 1));
                context_t sub_context = context.fork();
                for (std::size_t i = 0; i < expression.parameter_names.size(); ++i)
                {
                    expression_t param;
                    param.name = expression.parameter_names[i];
                    param.content = resolve(func_args[i+1], context);
                    sub_context.add_expression(param);
                }
                std::string pre = stuff.substr(0, fstart);
                std::string mid = resolve(expression.content, sub_context);
                std::string end = resolve(stuff.substr(fend, stuff.length() - fend), context);
                return pre + mid + end;
            }
            else
            {
                throw sdf_exception("internal error: no arguments in invocation alias");
            }
        }
        else
        {
            std::vector<std::string> resolved_args;
            for (const auto& arg: func_args) resolved_args.push_back(resolve(arg, context));
            const std::string rcontent = [&]()
            {
                try
                {
                    const auto& function = context.find_function(func_name);
                    return function(resolved_args);
                }
                catch (const std::exception& e)
                {
                    throw sdf_exception("error evaluating function invocation \""
                    + stuff + "\":\nfailed to evaluate function \"" + func_name + "\":\n" + e.what());
                }
            }();
            return stuff.substr(0, fstart) + rcontent + resolve(stuff.substr(fend, stuff.length() - fend), context);
        }
    }
}
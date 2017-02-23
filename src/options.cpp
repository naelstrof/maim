#include "options.hpp"

Options::Options( int argc, char** argv ) {
    validArguments.push_back( Argument( "bordersize",   'b', false ) );
    validArguments.push_back( Argument( "padding",      'p', false ) );
    validArguments.push_back( Argument( "color",        'c', false ) );
    validArguments.push_back( Argument( "shader",       'r', false ) );
    validArguments.push_back( Argument( "highlight",    'h', true ) );
    validArguments.push_back( Argument( "format",       'f', false ) );
    validArguments.push_back( Argument( "tolerance",    't', false ) );
    validArguments.push_back( Argument( "nodecorations", 'n', false ) );
    validArguments.push_back( Argument( "nokeyboard",   'k', true ) );
    validArguments.push_back( Argument( "help",         'h', true ) );
    validArguments.push_back( Argument( "xdisplay",     'x', false ) );
    validArguments.push_back( Argument( "version",      'v', true ) );
    validArguments.push_back( Argument( "quiet",        'q', true ) );
    validArguments.push_back( Argument( "window",       'i', false ) );
    validArguments.push_back( Argument( "geometry",     'g', false ) );
    validArguments.push_back( Argument( "delay",        'd', false ) );
    validArguments.push_back( Argument( "hidecursor",   'u', true ) );
    validArguments.push_back( Argument( "select",       's', true ) );
    validArguments.push_back( Argument( "quality",      'm', false ) );
    validate( argc, argv );
}


int Options::validateStringOption( int argc, char** argv, int argumentIndex ) {
    std::string argument = argv[argumentIndex];
    unsigned int index = 0;
    while( index < validArguments.size() ) {
        Argument& check = validArguments[index];
        for( unsigned int i=0;i<check.name.length();i++ ) {
            if ( check.name[i] != argument[i+2] ) {
                break;
            }
            if ( i == check.name.length()-1 ) {
                if ( !check.isFlagArgument && argument.find("=") == std::string::npos ) {
                    throw new std::invalid_argument("Expected `=` after " + arguments[i]);
                }
                if ( check.isFlagArgument && i+3 != argument.length() ) {
                    throw new std::invalid_argument("Trailing characters on flag " + argument );
                }
                return parseStringOption( argc, argv, argumentIndex, index );
            }
        }
        index++;
    }
    throw new std::invalid_argument("Invalid argument " + argument);
    return 0;
}

int Options::parseCharOption( int argc, char** argv, int argumentIndex, int validIndex ) {
    std::string argument = argv[argumentIndex];
    Argument& check = validArguments[validIndex];
    // If we're a flag, we take no arguments, nor do we allow = signs or whatever.
    if ( check.isFlagArgument ) {
        if ( argument != std::string()+"-"+check.cname ) {
            for( int o=1;o<argument.length();o++ ) {
                bool isValid = false;
                for( int i=0;i<validArguments.size()&&!isValid;i++ ) {
                    if ( argument[o] == check.cname ) {
                        if ( check.isFlagArgument ) {
                            isValid = true;
                            arguments.push_back( std::string()+check.cname );
                            values.push_back("");
                            break;
                        } else {
                            throw new std::invalid_argument( std::string()+"Truncating non-flag arguments is not allowed. Split this up: `" + argument + "`." );
                        }
                    }
                }
                if (!isValid) {
                    throw new std::invalid_argument( std::string()+"Unexpected characters around flag `" + argument + "`." );
                }
            }
            return 1;
        } else {
            arguments.push_back( std::string()+argument[1] );
            values.push_back("");
            return 1;
        }
    }
    arguments.push_back( std::string()+argument[1] );
    // If they supplied the parameters with spaces
    if ( argument == std::string()+"-"+check.cname ) {
        values.push_back(argv[argumentIndex+1]);
        return 2;
    }
    // If they didn't supply the parameters with spaces
    if ( argument[2] == '=' ) {
        values.push_back(argument.substr(3));
        return 1;
    }
    values.push_back(argument.substr(2));
    return 1;
}

int Options::parseStringOption( int argc, char** argv, int argumentIndex, int validIndex ) {
    std::string argument = argv[argumentIndex];
    if ( validArguments[validIndex].isFlagArgument ) {
        arguments.push_back( argument.substr(2) );
        values.push_back("");
        return 1;
    }
    arguments.push_back( argument.substr(2,argument.find_first_of('=')) );
    values.push_back(argument.substr(argument.find_first_of('=')));
    return 1;
}

int Options::validateCharOption( int argc, char** argv, int argumentIndex ) {
    std::string argument = argv[argumentIndex];
    unsigned int index = 0;
    while( index < validArguments.size() ) {
        char check = validArguments[index].cname;
        if ( argument.length() < 2 ) {
            continue;
        }
        if ( check == argument[1] && ( argument.find("=") == 2 || argument.find('=') == std::string::npos ) && argument[0] == '-' ) {
            return parseCharOption( argc, argv, argumentIndex, index );
        }
        index++;
    }
    throw new std::invalid_argument("Invalid argument `" + argument + "`.");
    return 0;
}

void Options::validate( int argc, char** argv ) {
    for ( int i=1;i<argc;) {
        std::string argument = argv[i];
        if ( argument[0] != '-' ) {
            floatingValues.push_back( argument );
            if ( floatingValues.size() > maxFloatingValues ) {
                throw new std::invalid_argument("Unexpected floating value `" + argument + "`. Forget to specify an option?" );
            }
            i++;
            continue;
        }
        if ( argument[0] == '-' && argument[1] == '-' ) {
            i += validateStringOption( argc, argv, i );
            continue;
        }
        i += validateCharOption( argc, argv, i );
    }
}

bool Options::getFloat( std::string name, char namec, float& found ) {
    for( unsigned int i=0;i<arguments.size();i++ ) {
        if ( arguments[i] == name || arguments[i] == std::string("")+namec ) {
            std::string::size_type sz;
            float retvar;
            try {
                retvar = std::stof(values[i],&sz);
            } catch ( ... ) {
                throw new std::invalid_argument("Unable to parse `" + arguments[i] + "`'s value `" + values[i] + "` as a float.");
            }
            if ( sz != values[i].length() ) {
                throw new std::invalid_argument("Unable to parse `" + arguments[i] + "`'s value `" + values[i] + "` as a float.");
            }
            found = retvar;
            return true;
        }
    }
    return false;
}

bool Options::getInt( std::string name, char namec, int& found ) {
    for( unsigned int i=0;i<arguments.size();i++ ) {
        if ( arguments[i] == name || arguments[i] == std::string("")+namec ) {
            if ( arguments[i].size() > 1 && arguments[i].find("=") == std::string::npos ) {
                throw new std::invalid_argument("Expected `=` after " + arguments[i]);
            }
            std::string::size_type sz;
            float retvar;
            try {
                retvar = std::stoi(values[i],&sz);
            } catch ( ... ) {
                throw new std::invalid_argument("Unable to parse " + arguments[i] + "'s value " + values[i] + " as an integer.");
            }
            if ( sz != values[i].length() ) {
                throw new std::invalid_argument("Unable to parse " + arguments[i] + "'s value " + values[i] + " as an integer.");
            }
            found = retvar;
            return true;
        }
    }
    return false;
}

bool Options::getString( std::string name, char namec, std::string& found ) {
    for( unsigned int i=0;i<arguments.size();i++ ) {
        if ( arguments[i] == name || arguments[i] == std::string("")+namec ) {
            found = values[i];
            return true;
        }
    }
    return false;
}

bool Options::getColor( std::string name, char namec, glm::vec4& found ) {
    for( unsigned int i=0;i<arguments.size();i++ ) {
        if ( arguments[i] == name || arguments[i] == std::string("")+namec ) {
            std::string::size_type sz;
            std::string value = values[i];
            try {
                found[0] = std::stof(value,&sz);
                value = value.substr(sz+1);
                found[1] = std::stof(value,&sz);
                value = value.substr(sz+1);
                found[2] = std::stof(value,&sz);
                if ( value.size() != sz ) {
                    value = value.substr(sz+1);
                    found[3] = std::stof(value,&sz);
                    if ( value.size() != sz ) {
                        throw "dur";
                    }
                } else {
                    found[3] = 1;
                }
            } catch ( ... ) {
                throw new std::invalid_argument("Unable to parse `" + arguments[i] + "`'s value `" + values[i] + "` as a color. Should be in the format r,g,b or r,g,b,a. Like 1,1,1,1.");
            }
            return true;
        }
    }
    return false;
}

bool Options::getBool( std::string name, char namec, bool& found ) {
    for( unsigned int i=0;i<arguments.size();i++ ) {
        if ( arguments[i] == name || arguments[i] == std::string("")+namec ) {
            if ( values[i] != "" ) {
                throw new std::invalid_argument("Unexpected value `" + values[i] + "` for flag argument `" + arguments[i] + "`.");
            }
            found = true;
            return true;
        }
    }
    return false;
}

bool Options::getGeometry( std::string name, char namec, glm::vec4& found ) {
    for( unsigned int i=0;i<arguments.size();i++ ) {
        if ( arguments[i] == name || arguments[i] == std::string("")+namec ) {
            std::string::size_type sz = 0;
            std::string value = values[i];
            glm::vec2 dim(0,0);
            int curpos = 0;
            glm::vec2 pos(0,0);
            try {
                if ( std::count(value.begin(), value.end(), '+') > 2 ) {
                    throw "dur";
                }
                if ( std::count(value.begin(), value.end(), '-') > 2 ) {
                    throw "dur";
                }
                if ( std::count(value.begin(), value.end(), 'x') > 1 ) {
                    throw "dur";
                }
                while( value != "" ) {
                    switch( value[0] ) {
                        case 'x':
                            dim.y = std::stof(value.substr(1),&sz);
                            sz++;
                            break;
                        case '+':
                            pos[curpos++] = std::stof(value.substr(1), &sz);
                            sz++;
                            break;
                        case '-':
                            pos[curpos++] = -std::stof(value.substr(1), &sz);
                            sz++;
                            break;
                        default:
                            dim.x = std::stof(value,&sz);
                            break;
                    }
                    value = value.substr(sz);
                }
            } catch ( ... ) {
                throw new std::invalid_argument("Unable to parse `" + arguments[i] + "`'s value `" + values[i] + "` as a geometry. Should be in the format wxh+x+y, +x+y, or wxh. Like 600x400+10+20");
            }
            found.x = pos.x;
            found.y = pos.y;
            found.z = dim.x;
            found.w = dim.y;
            return true;
        }
    }
    return false;
}

bool Options::getWindow( std::string name, char namec, Window& found ) {
    for( unsigned int i=0;i<arguments.size();i++ ) {
        if ( arguments[i] == name || arguments[i] == std::string("")+namec ) {
            if ( arguments[i].size() > 1 && arguments[i].find("=") == std::string::npos ) {
                throw new std::invalid_argument("Expected `=` after " + arguments[i]);
            }
            std::string::size_type sz;
            float retvar;
            try {
                retvar = std::stoi(values[i],&sz);
            } catch ( ... ) {
                try {
                    retvar = std::stoul(values[i],&sz,16);
                } catch ( ... ) {
                    throw new std::invalid_argument("Unable to parse " + arguments[i] + "'s value " + values[i] + " as an integer or hex.");
                }
            }
            if ( sz != values[i].length() ) {
                throw new std::invalid_argument("Unable to parse " + arguments[i] + "'s value " + values[i] + " as an integer or hex.");
            }
            found = retvar;
            return true;
        }
    }
    return false;
}

bool Options::getFloatingString( int index, std::string& found ) {
    if ( index >= 0 && index < floatingValues.size() && floatingValues.size() > 0 ) {
        found = floatingValues[index];
        return true;
    }
    return false;
}

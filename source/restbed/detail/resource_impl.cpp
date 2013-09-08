/*
 * Site: restbed.corvusoft.co.uk
 * Author: Ben Crowhurst
 *
 * Copyright (c) 2013 Restbed Core Development Team and Community Contributors
 *
 * This file is part of Restbed.
 *
 * Restbed is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Restbed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with Restbed.  If not, see <http://www.gnu.org/licenses/>.
 */

//System Includes

//Project Includes
#include "restbed/string.h"
#include "restbed/method.h"
#include "restbed/request.h"
#include "restbed/response.h"
#include "restbed/status_code.h"
#include "restbed/detail/resource_impl.h"

//External Includes

//System Namespaces
using std::map;
using std::bind;
using std::string;
using std::function;
using std::placeholders::_1;

//Project Namespaces

//External Namespaces

namespace restbed
{
    namespace detail
    {
        ResourceImpl::ResourceImpl( void ) : m_path( ".*" ),
                                             m_content_type( ".*" ),
                                             m_method_handlers( )

        {
            setup( );
        }
        
        ResourceImpl::ResourceImpl( const ResourceImpl& original ) : m_path( original.m_path ),
                                                                     m_content_type( original.m_content_type ),
                                                                     m_method_handlers( original.m_method_handlers )
        {
            //n/a
        }
        
        ResourceImpl::~ResourceImpl( void )
        {
            //n/a
        }

        string ResourceImpl::get_path( void ) const
        {
            return m_path;
        }
        
        string ResourceImpl::get_content_type( void ) const
        {
            return m_content_type;
        }

        function< Response ( const Request& ) > ResourceImpl::get_method_handler( const Method& method ) const
        {
            return m_method_handlers.at( method.to_string( ) );
        }

        map< string, function< Response ( const Request& ) > > ResourceImpl::get_method_handlers( void ) const
        {
            return m_method_handlers;
        }

        void ResourceImpl::set_path( const string& value )
        {
            m_path = value;
        }

        void ResourceImpl::set_content_type( const string& value )
        {
            m_content_type = value;
        }

        void ResourceImpl::set_method_handler( const Method& method, const function< Response ( const Request& ) >& callback )
        {
            string key = String::to_upper( method.to_string( ) );

            m_method_handlers[ key ] = callback;
        }
        
        bool ResourceImpl::operator <( const ResourceImpl& rhs ) const
        {
            return m_path < rhs.m_path;
        }
        
        bool ResourceImpl::operator >( const ResourceImpl& rhs ) const
        {
            return m_path > rhs.m_path;
        }
        
        bool ResourceImpl::operator ==( const ResourceImpl& rhs ) const
        {
            return m_path == rhs.m_path;
        }
        
        bool ResourceImpl::operator !=( const ResourceImpl& rhs ) const
        {
            return m_path not_eq rhs.m_path;
        }

        ResourceImpl& ResourceImpl::operator =( const ResourceImpl& rhs )
        {
            m_path = rhs.m_path;

            m_content_type = rhs.m_content_type;

            m_method_handlers = rhs.m_method_handlers;

            return *this;
        }

        void ResourceImpl::setup( void )
        {
            set_method_handler( "GET",     &ResourceImpl::default_handler );
            set_method_handler( "PUT",     &ResourceImpl::default_handler );
            set_method_handler( "POST",    &ResourceImpl::default_handler );
            set_method_handler( "HEAD",    &ResourceImpl::default_handler );
            set_method_handler( "DELETE",  &ResourceImpl::default_handler );
            set_method_handler( "CONNECT", &ResourceImpl::default_handler );
            set_method_handler( "TRACE",   &ResourceImpl::default_trace_handler );
            set_method_handler( "OPTIONS", bind( &ResourceImpl::default_options_handler, this, _1 ) );
        }

        template<typename T, typename... U>
        size_t getAddress(std::function<T(U...)> f)
        {
            typedef T( fnType )( U... );

            fnType ** fnPointer = f.template target< fnType* >( );

            size_t address = 0;

            if ( fnPointer not_eq nullptr )
            {
                address = ( size_t )*fnPointer;
            }

            return address;
        }

        string ResourceImpl::generate_allow_header_value( void )
        {
            string value = String::empty;

            for ( auto& handler : m_method_handlers )
            {
                address_type callback_address = getAddress( handler.second );
                address_type default_address = ( address_type ) ResourceImpl::default_handler;

                if ( callback_address not_eq default_address )
                {
                    string method = handler.first;

                    value += method + ", ";
                }
            }

            if ( not value.empty( ) )
            {
                value = value.substr( 0, value.length( ) - 2 );
            }

            return value;   
        }

        Response ResourceImpl::default_options_handler( const Request& request )
        {
            Response response;
            response.set_status_code( StatusCode::OK );
            response.set_header( "Allow", generate_allow_header_value( ) );

            std::cout << "options handler: allow: " << generate_allow_header_value() << std::endl;
            
            return response;
        }

        Response ResourceImpl::default_handler( const Request& request ) //not_implemented_handler //make public!
        {
            Response response;
            response.set_status_code( StatusCode::NOT_IMPLEMENTED );

            return response;
        }
        
        Response ResourceImpl::default_trace_handler( const Request& request )
        {
            Response response;
            response.set_status_code( StatusCode::OK );
            response.set_header( "Content-Type", "message/http" );

            //string body = "TRACE <res> HTTP/<ver>" + \r\n + "Host: this.machine.com"
            //response.set_body( request.get_body( ) );
            
            return response;
        }
    }
}

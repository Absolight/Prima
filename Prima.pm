#
#  Copyright (c) 1997-1999 The Protein Laboratory, University of Copenhagen
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
#  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.
#
package Prima;

use strict;
require DynaLoader;
use vars qw($VERSION @ISA);
@ISA = qw(DynaLoader);

$VERSION = '0.01';
bootstrap Prima $VERSION;
$::application = undef;
require Prima::Const;
require Prima::Classes;
init Prima $VERSION;

sub END
{
   &Prima::cleanup();
}

sub run
{
   die "Prima was not properly initialized\n" unless defined $::application;
   $::application-> go if $::application-> alive;
}

sub find_image
{
   shift if @_ > 1;
   my $name = shift;
   $name =~ s!::!/!g;
   for (@INC) {
      return "$_/Prima/images/$name" if -f "$_/Prima/images/$name" && -r _;
   }
   return undef;
}

sub import
{
   my @module = @_;
   while (@module) {
      my $module = shift @module;
      my %parameters = ();
      %parameters = %{shift @module} if @module && ref($module[0]) eq 'HASH';
      $module = "Prima::$module" unless $module =~ /^Prima::/;
      eval "use $module \%parameters;" if $module;
   }
}

1;

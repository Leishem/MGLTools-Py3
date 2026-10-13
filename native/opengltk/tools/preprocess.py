#
# The contents of this file are subject to the Mozilla Public
# License Version 1.1 (the "License"); you may not use this file
# except in compliance with the License. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
# 
# Software distributed under the License is distributed on an "AS
# IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
# implied. See the License for the specific language governing
# rights and limitations under the License.
# The Original Code is "Java-Python Extension libplus (JPE-libplus)".
# 
# The Initial Developer of the Original Code is Frederic Bruno Giacometti.
# Portions created by Frederic Bruno Giacometti are
# Copyright (C) 2001-2002 Frederic Bruno Giacometti. All Rights Reserved.
# 
# Contributor(s): frederic.giacometti@arakne.com
# 
# Acknowledgments:
# Particular gratitude is expressed to the following parties for their
# contributing support to the development of JPE-libplus:
#     - The Molecular Graphics Laboratory (MGL)
#       at The Scripps Research Institute (TSRI), in La Jolla, CA, USA;
#       and in particular to Michel Sanner and Arthur Olson.
#

__all__ = [ 'removeextern']

import re

def removeextern( txt, matches='extern'):
    return re.compile( r'^\s*(%s)\s*(\w+.*?\()' % matches,
                      re.MULTILINE).sub( lambda x: x.group( 2), txt)

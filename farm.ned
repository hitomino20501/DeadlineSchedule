//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
// database.ned slave°}¦C ³£­n§ï
network Farm{
	submodules:
	    database:Database{
    		@display("p=325,50");
		}
	    slave[100]:Slave{
    		@display("p=100,150,m,10,50,50");
		}
	    workstation:Workstation{
    		@display("p=200,50");
		}
		draw:Draw{
			@display("p=750,50");
		}
	connections:
	    for i=0..99{
	    	database.out[i] --> {  delay = 100ms; } --> slave[i].in;
	    	database.in[i] <-- {  delay = 100ms; } <-- slave[i].out;
	    }
	    database.out[100] --> {  delay = 100ms; } --> workstation.in;
	    database.in[100] <-- {  delay = 100ms; } <-- workstation.out;
}

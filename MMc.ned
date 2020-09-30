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

simple SourceMMc
{
    parameters:
        @display("i=block/source;s=source");
        //double ArrivalRate;

    gates:
        output out;
}

simple QueueMMc
{
    parameters:
        @display("i=block/queue;q=queue");

    gates:
        input inSrc;
        inout inSrv[];
}

simple ServerMMc
{
    parameters:
        //double ServiceRate;
        @display("i=block/server;s=server");

    gates:
        inout inQue;
}

network MMcServer
{
    submodules:
        Source: SourceMMc {
            @display("p=32,36");
        }
        Queue: QueueMMc {
            @display("p=119,36");
        }
        Server[5]: ServerMMc {
            @display("p=233,36");
        }

    connections:
        Source.out --> Queue.inSrc;
        for i=0..5-1{
            Server[i].inQue <--> Queue.inSrv++;
        }
}
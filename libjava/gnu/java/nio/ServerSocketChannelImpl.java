/* ServerSocketChannelImpl.java -- 
   Copyright (C) 2002 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package gnu.java.nio;

import gnu.java.net.PlainSocketImpl;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.channels.spi.SelectorProvider;

public final class ServerSocketChannelImpl extends ServerSocketChannel
{
  ServerSocket serverSocket;
  PlainSocketImpl impl;
  boolean blocking = true;
  boolean connected = false;

  protected ServerSocketChannelImpl (SelectorProvider provider)
    throws IOException
  {
    super (provider);
    impl = new PlainSocketImpl();
    initServerSocket();
  }

  /*
   * This method is only need to call a package private constructor
   * of java.net.ServerSocket. It only initializes the member variables
   * "serverSocket".
   */
  private native void initServerSocket() throws IOException;

  public int getNativeFD()
  {
    return impl.getNativeFD();
  }
 
  public void finalizer()
  {
    if (connected)
      {
        try
          {
            close ();
          }
        catch (Exception e)
          {
          }
      }
  }

  protected void implCloseSelectableChannel () throws IOException
  {
    connected = false;
    serverSocket.close();
  }

  protected void implConfigureBlocking (boolean blocking) throws IOException
  {
    this.blocking = blocking; // FIXME
  }

  public SocketChannel accept () throws IOException
  {
    SocketChannelImpl result = new SocketChannelImpl (provider ());
    Socket socket = serverSocket.accept();
    //socket.setChannel (result); // FIXME
    return result;
  }

  public ServerSocket socket ()
  {
    return serverSocket;
  }
}

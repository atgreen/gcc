/* java.beans.PropertyChangeSupport
   Copyright (C) 1998, 1999 Free Software Foundation, Inc.

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

As a special exception, if you link this library with other files to
produce an executable, this library does not by itself cause the
resulting executable to be covered by the GNU General Public License.
This exception does not however invalidate any other reasons why the
executable file might be covered by the GNU General Public License. */


package java.beans;
import java.util.Hashtable;
import java.util.Vector;

/**
 ** PropertyChangeSupport makes it easy to fire property
 ** change events and handle listeners.
 **
 ** @author John Keiser
 ** @since JDK1.1
 ** @version 1.2.0, 15 Mar 1999
 **/

public class PropertyChangeSupport implements java.io.Serializable {
	Hashtable propertyListeners = new Hashtable();
	Vector listeners = new Vector();
	Object bean;

	/** Create PropertyChangeSupport to work with a specific
	 ** source bean.
	 ** @param bean the source bean to use.
	 **/
	public PropertyChangeSupport(Object bean) {
		this.bean = bean;
	}

	/** Adds a PropertyChangeListener to the list of listeners.
	 ** All property change events will be sent to this listener.
	 ** <P>
	 **
	 ** The listener add is not unique: that is, <em>n</em> adds with
	 ** the same listener will result in <em>n</em> events being sent
	 ** to that listener for every property change.
	 ** <P>
	 **
	 ** Adding a null listener will cause undefined behavior.
	 **
	 ** @param l the listener to add.
	 **/
	public void addPropertyChangeListener(PropertyChangeListener l) {
		listeners.addElement(l);
	}

	/** Adds a PropertyChangeListener listening on the specified property.
	 ** Events will be sent to the listener for that particular property.
	 ** <P>
	 **
	 ** The listener add is not unique; that is, <em>n</em> adds on a
	 ** particular property for a particular listener will result in
	 ** <em>n</em> events being sent to that listener when that
	 ** property is changed.
	 ** <P>
	 **
	 ** The effect is cumulative, too; if you are registered to listen
	 ** to receive events on all property changes, and then you
	 ** register on a particular property, you will receive change
	 ** events for that property twice.
	 ** <P>
	 **
	 ** Adding a null listener will cause undefined behavior.
	 **
	 ** @param propertyName the name of the property to listen on.
	 ** @param l the listener to add.
	 **/
	public void addPropertyChangeListener(String propertyName, PropertyChangeListener l) {
		synchronized(propertyListeners) {
			Vector v = (Vector)propertyListeners.get(propertyName);
			try {
				v.addElement(l);
			} catch(NullPointerException e) {
				/* If v is not found, create a new vector. */
				v = new Vector();
				v.addElement(l);
				propertyListeners.put(propertyName, v);
			}
		}
	}

	/** Removes a PropertyChangeListener from the list of listeners.
	 ** If any specific properties are being listened on, they must
	 ** be deregistered by themselves; this will only remove the
	 ** general listener to all properties.
	 ** <P>
	 **
	 ** If <code>add()</code> has been called multiple times for a
	 ** particular listener, <code>remove()</code> will have to be
	 ** called the same number of times to deregister it.
	 **
	 ** @param l the listener to remove.
	 **/
	public void removePropertyChangeListener(PropertyChangeListener l) {
		listeners.removeElement(l);
	}

	/** Removes a PropertyChangeListener from listening to a specific property.
	 ** <P>
	 **
	 ** If <code>add()</code> has been called multiple times for a
	 ** particular listener on a property, <code>remove()</code> will
	 ** have to be called the same number of times to deregister it.
	 **
	 ** @param propertyName the property to stop listening on.
	 ** @param l the listener to remove.
	 **/
	public void removePropertyChangeListener(String propertyName, PropertyChangeListener l) {
		synchronized(propertyListeners) {
			Vector v = (Vector)propertyListeners.get(propertyName);
			try {
				v.removeElement(l);
				if(v.size() == 0) {
					propertyListeners.remove(propertyName);
				}
			} catch(NullPointerException e) {
				/* if v is not found, do nothing. */
			}
		}
	}

	/** Fire a PropertyChangeEvent to all the listeners.
	 **
	 ** @param event the event to fire.
	 **/
	public void firePropertyChange(PropertyChangeEvent event) {
		for(int i=0;i<listeners.size();i++) {
			((PropertyChangeListener)listeners.elementAt(i)).propertyChange(event);
		}
		Vector moreListeners = (Vector)propertyListeners.get(event.getPropertyName());
		if(moreListeners != null) {
			for(int i=0;i<moreListeners.size();i++) {
				((PropertyChangeListener)moreListeners.elementAt(i)).propertyChange(event);
			}
		}
	}

	/** Fire a PropertyChangeEvent containing the old and new values of the property to all the listeners.
	 **
	 ** @param propertyName the name of the property that changed.
	 ** @param oldVal the old value.
	 ** @param newVal the new value.
	 **/
	public void firePropertyChange(String propertyName, Object oldVal, Object newVal) {
		firePropertyChange(new PropertyChangeEvent(bean,propertyName,oldVal,newVal));
	}

	/** Fire a PropertyChangeEvent containing the old and new values of the property to all the listeners.
	 **
	 ** @param propertyName the name of the property that changed.
	 ** @param oldVal the old value.
	 ** @param newVal the new value.
	 **/
	public void firePropertyChange(String propertyName, boolean oldVal, boolean newVal) {
		firePropertyChange(new PropertyChangeEvent(bean, propertyName, new Boolean(oldVal), new Boolean(newVal)));
	}

	/** Fire a PropertyChangeEvent containing the old and new values of the property to all the listeners.
	 **
	 ** @param propertyName the name of the property that changed.
	 ** @param oldVal the old value.
	 ** @param newVal the new value.
	 **/
	public void firePropertyChange(String propertyName, int oldVal, int newVal) {
		firePropertyChange(new PropertyChangeEvent(bean, propertyName, new Integer(oldVal), new Integer(newVal)));
	}

	/** Tell whether the specified property is being listened on or not.
	 ** This will only return <code>true</code> if there are listeners
	 ** on all properties or if there is a listener specifically on this
	 ** property.
	 **
	 ** @param propertyName the property that may be listened on
	 ** @return whether the property is being listened on
	 **/
	 public boolean hasListeners(String propertyName) {
	 	return listeners.size() > 0  || propertyListeners.get(propertyName) != null;
	 }
}

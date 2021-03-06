#ifndef Oberon_Editor_Outline_h
#define Oberon_Editor_Outline_h
/*
    This file is part of Oberon.

    Copyright (c) 2019-2020 Marco Melorio

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <gtkmm/builder.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>

#include "Oberon/Oberon.h"
#include "Oberon/Editor/Editor.h"

namespace Oberon { namespace Editor {

class Outline: public Gtk::TreeView {
    public:
        explicit Outline(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Properties* properties);

        void updateWithSceneData(SceneData& data);

        std::vector<UnsignedInt>& selectedObjects() { return _selectedObjects; }

    private:
        void onRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*);
        void onButtonPressEvent(GdkEventButton* buttonEvent);

        void onAddChildItemActivate();
        void onDeleteItemActivate();

        void addObjectRow(const Gtk::TreeModel::Row& parentRow, UnsignedInt objectId);

    private:
        struct ModelColumns: public Gtk::TreeModel::ColumnRecord {
            explicit ModelColumns() { add(name); add(objectId); }

            Gtk::TreeModelColumn<std::string> name;
            Gtk::TreeModelColumn<UnsignedInt> objectId;
        };

        ModelColumns _columns;
        Glib::RefPtr<Gtk::TreeStore> _treeStore;
        Gtk::Menu* _menuPopup;

        SceneData* _sceneData;

        Properties* _properties;

        std::vector<UnsignedInt> _selectedObjects;
};

}}

#endif

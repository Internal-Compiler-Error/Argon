#include "argon_app.hpp"

#include "download_file_chooser.hpp"
#include "download_link_input.hpp"

#include <gtkmm/aboutdialog.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/main.h>
#include <memory>
#include <vector>

using namespace Argon::gui;
using namespace Gtk;
using namespace std;

shared_ptr<argon_app> argon_app::instance;

argon_app::argon_app()
    : ApplicationWindow{}
    , download{ "download" }
    , submenu_new_download{ "new download" }
    , submenu_new_download_by_url{ "new download by url" }
    , about{ "about" }
    , submenu_about_about{ "about" }
    , scheduler{ network::download_scheduler::get_instance() }
{
    // set up basic size_
    set_title("Argon Downloader");
    //    set_icon(Gdk::Pixbuf::create_from_file("logo.jpg", 200, 400));
    resize(800, 400);

    add(box);

    // set up menu bar
    box.pack_start(menu_bar, PACK_SHRINK);

    // construct the menu bar
    menu_bar.append(download);
    download.set_submenu(submenu_download);
    submenu_download.append(submenu_new_download);
    submenu_download.append(submenu_new_download_by_url);

    menu_bar.append(about);
    about.set_submenu(submenu_about);
    submenu_about.append(submenu_about_about);

    submenu_about_about.signal_activate().connect([]() {
        AboutDialog about_dialog{};

        about_dialog.set_logo(Gdk::Pixbuf::create_from_file("logo.jpg", 200, 400));

        about_dialog.set_authors(std::vector<Glib::ustring>{ "Bill Wang" });

        about_dialog.set_copyright("Bill Wang");
        about_dialog.set_license_type(Gtk::LICENSE_GPL_2_0);

        about_dialog.set_version("0.0.1");

        about_dialog.set_website("http://www.billwang2001.com");
        about_dialog.set_website_label("Argon Official Website http://www.billwang2001.com");

        about_dialog.set_comments("Argon is a high performance downloader written in modern C++");
        about_dialog.run();
    });

    submenu_new_download_by_url.signal_activate().connect([]() {
        download_link_input input;
        Gtk::Main::run(input);
    });

    submenu_new_download.signal_activate().connect([&]() {
        FileChooserDialog dialog{ "please select a file", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER };
        dialog.set_transient_for(*this);
        dialog.run();
    });

    // set up tree view
    scrolled_window.add(view);

    // Only show the scrollbars when they are necessary:
    scrolled_window.set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC);

    box.pack_start(scrolled_window, PACK_SHRINK);

    // create columns for main display
    view.append_column("index_", columns.index_);
    view.append_column("location_", columns.link_);
    view.append_column("ip_", columns.ip_);
    view.append_column("protocol", columns.protocol_);
    view.append_column("target_", columns.target_);
    auto const last = view.append_column("progress", cell_renderer_progress);

    auto last_column = view.get_column(last - 1);
    last_column->add_attribute(cell_renderer_progress.property_value(), columns.progress_);

    for (auto i = 0; i < last; ++i)
    {
        auto col = view.get_column(i);

        col->set_expand();
        col->set_resizable();
    }

    columns_ptr = ListStore::create(columns);
    view.set_model(columns_ptr);
    show_all_children();
}
std::shared_ptr<argon_app> argon_app::get_instance()
{
    if (!instance)
    {
        instance = make_shared<argon_app>();
    }
    return instance;
}

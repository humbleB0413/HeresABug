/**
 * @file navigation_controller.hpp
 * @author humbleB0413 (humbleb0413@gmail.com)
 * @brief QObject wrapper exposing page navigation (which page.qml is currently shown) to QML
 * @version 0.1
 * @date 2026-08-25
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include "page_registry.hpp"

#include <QObject>
#include <QString>

#include <string>
#include <vector>

/**
 * @brief Tracks which page id/type is active plus how it was reached (a linear back-history) and
 *        the page recording was last paused on. Only says *which* page is active — turning that id
 *        into a rendered page is PageLoader.qml's job. Registered in main.cpp as the
 *        "navigation_controller" context property.
 */
class navigation_controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString current_page_type READ current_page_type NOTIFY current_page_changed)
    Q_PROPERTY(QString current_page_id READ current_page_id NOTIFY current_page_changed)

private:
    page_registry pages;
    std::string current_id;
    std::vector<std::string> history;

    // The page recording was last stopped on. mark_recording_anchor() stamps it; a subsequent
    // resume_to_recording_anchor() walks the back-stack (pop, never push) until it's current again,
    // so pages visited while stopped are unwound rather than piled on top. Cleared by
    // go_to_initial_page() (a fresh session doesn't resume anywhere).
    std::string recording_anchor_id;

    /**
     * @brief Apply a page as current: validate it against `pages`, set current_id, emit
     *        current_page_changed. Does NOT touch history.
     * @param page_id  target page id
     * @return true if page_id is known; false (with a warning, state untouched) if not.
     */
    bool switch_to(const std::string& page_id);

    /**
     * @brief Make page_id current by popping the back-stack (never pushing) until it is current;
     *        if it isn't behind us in history, land on it directly, still without pushing.
     * @param page_id  target page id
     */
    void unwind_to(const std::string& page_id);

public:
    explicit navigation_controller(QObject* parent = nullptr);
    ~navigation_controller();

    /**
     * @brief Load the page_id -> qml-type config (see page_registry) and move to its initial page.
     * @param path  filesystem/resource path to the JSON config (e.g. ":/pages.json").
     */
    Q_INVOKABLE void load_page_config(const QString& path);

    /**
     * @brief Switch the active page, pushing the page navigated away from onto the back-history,
     *        then emit navigated_forward with that page's id.
     * @param page_id  target page id; no-op (with a warning) if not present in the loaded config.
     */
    Q_INVOKABLE void go_to_page(const QString& page_id);

    /**
     * @brief Return to the page active before the current one, popping the back-history.
     *        No-op (with a warning) if the history is empty.
     * @return true if the page changed (BackButton.qml tags its logged event only then).
     */
    Q_INVOKABLE bool go_back();

    /**
     * @brief Return to page_id. Used by Delete One to undo the page move a deleted event made: a
     *        NavButton's forward move is undone by unwinding the back-history (see unwind_to); a
     *        BackButton's move, whose page_id go_back() already popped off the history, is undone
     *        by moving forward again, pushing the current page.
     * @param page_id  page to return to; "" (the deleted event didn't navigate) is a silent no-op.
     */
    Q_INVOKABLE void return_to_page(const QString& page_id);

    /**
     * @brief Clear the back-history AND the recording anchor, emit session_reset, then jump to the
     *        config's initial page. Used to begin a fresh session (and by ExportButton /
     *        ClearRecordButton). No-op (with a warning) if the config has no initial page.
     */
    Q_INVOKABLE void go_to_initial_page();

    /** @brief Stamp the current page as the point recording was stopped on. Call when recording turns off. */
    Q_INVOKABLE void mark_recording_anchor();

    /**
     * @brief Return to the page recording was stopped on by popping the back-stack (never pushing).
     *        Falls back to go_to_initial_page() if there is no anchor yet. Call when recording turns on.
     */
    Q_INVOKABLE void resume_to_recording_anchor();

    /** @return the QML type name of the active page, or "" if none. */
    QString current_page_type() const;
    /** @return the id of the active page, or "" if none. */
    QString current_page_id() const;

signals:
    void current_page_changed();
    /**
     * @brief go_to_page() moved forward, the only navigation a logged event can cause. Not emitted
     *        by go_back / resume / initial-page moves.
     * @param from_page_id  the page navigated away from
     */
    void navigated_forward(const QString& from_page_id);
    /** @brief A fresh session is starting: page UI state from the previous one should be discarded. */
    void session_reset();
};

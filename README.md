# top_down

const Plugin = require('@ckeditor/ckeditor5-core/src/plugin');
const ButtonView = require('@ckeditor/ckeditor5-ui/src/button/buttonview');

const ClickObserver = require('@ckeditor/ckeditor5-engine/src/view/observer/clickobserver');
const ContextualBalloon = require('@ckeditor/ckeditor5-ui/src/panel/balloon/contextualballoon');
const clickOutsideHandler = require('@ckeditor/ckeditor5-ui/src/bindings/clickoutsidehandler');

const ViewPopup = require('./ui/viewpopup');
const EditPopup = require('./ui/editpopup');

const bookmarkIcon = require('../theme/icons/bookmark.svg');

module.exports = class BookmarkUI extends Plugin {
    init() {
        const editor = this.editor;
        const t = editor.t;

        editor.ui.componentFactory.add('bookmark', locale => {
            const btn = new ButtonView(locale);
            btn.set({
                label: t('Bookmark'),
                withText: false,
                tooltip: true,
                icon: bookmarkIcon
            });

            const bookmarkCommand = editor.commands.get('insertBookmark');
            btn.bind('isEnabled').to(bookmarkCommand, 'isEnabled');
            btn.bind('isOn').to(bookmarkCommand, 'isBookmark');

            this.listenTo(btn, 'execute', () => {
                editor.execute('insertBookmark');
                this._showUI();
            });

            return btn;
        });

        this._balloon = editor.plugins.get(ContextualBalloon);

        this._editPopup = this._createEditPopup();
        this._viewPopup = this._createViewPopup();

        editor.editing.view.addObserver(ClickObserver);
        this._enableUserBalloonInteractions();
    }

    _createEditPopup() {
        const editor = this.editor;
        const editPopup = new EditPopup(editor.locale);

        const command = editor.commands.get('insertBookmark');
        editPopup.tbName.bind('value').to(command, 'value');

        editPopup.keystrokes.set('Esc', (data, cancel) => {
            this._hideUI();
            cancel();
        });

        this.listenTo(editPopup, 'submit', () => {
            const bookmarkName = editPopup.tbName.element.value;
            editor.execute('insertBookmark', bookmarkName);
            this._hideUI();
        });

        this.listenTo(editPopup, 'cancel', () => {
            this._hideUI();
        });

        return editPopup;
    }

    _createViewPopup() {
        const editor = this.editor;
        const viewPopup = new ViewPopup(editor.locale);

        const command = editor.commands.get('insertBookmark');
        viewPopup.lblName.bind('text').to(command, 'value');

        this.listenTo(viewPopup, 'edit', () => {
            this._balloon.remove(this._viewPopup);
            this._balloon.add({
                view: this._editPopup,
                position: this._getBalloonPositionData()
            });

            this._editPopup.tbName.select();
        });

        this.listenTo(viewPopup, 'delete', () => {
            this.editor.execute('deleteBookmark');
            this._hideUI();
        });

        viewPopup.keystrokes.set('Esc', (data, cancel) => {
            this._hideUI();
            cancel();
        });

        return viewPopup;
    }

    _enableUserBalloonInteractions() {
        const viewDocument = this.editor.editing.view.document;

        this.listenTo(viewDocument, 'click', () => {
            const elmBookmark = this._getSelectedBookmarkElement();
            if (elmBookmark) {
                this._showUI();
            }
        });

        clickOutsideHandler({
            emitter: this._editPopup,
            activator: () => this._balloon.visibleView === this

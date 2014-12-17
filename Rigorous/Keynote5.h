/*
 * Keynote5.h
 */

#import <AppKit/AppKit.h>
#import <ScriptingBridge/ScriptingBridge.h>


@class Keynote5Item, Keynote5Application, Keynote5Color, Keynote5Slideshow, Keynote5Window, Keynote5AttributeRun, Keynote5Character, Keynote5Paragraph, Keynote5Text, Keynote5Attachment, Keynote5Word, Keynote5AppTheme, Keynote5AppTransition, Keynote5DocTheme, Keynote5MasterSlide, Keynote5Slide, Keynote5SlideTransition, Keynote5PrintSettings;

enum Keynote5Savo {
	Keynote5SavoAsk = 'ask ' /* Ask the user whether or not to save the file. */,
	Keynote5SavoNo = 'no  ' /* Do not save the file. */,
	Keynote5SavoYes = 'yes ' /* Save the file. */
};
typedef enum Keynote5Savo Keynote5Savo;

enum Keynote5KCct {
	Keynote5KCctArea_2d = 'are2' /* two-dimensional area chart. */,
	Keynote5KCctArea_3d = 'are3' /* three-dimensional area chart */,
	Keynote5KCctHorizontal_bar_2d = 'hbr2' /* two-dimensional horizontal bar chart */,
	Keynote5KCctHorizontal_bar_3d = 'hbr3' /* three-dimensional horizontal bar chart */,
	Keynote5KCctLine_2d = 'lin2' /*  two-dimensional line chart. */,
	Keynote5KCctLine_3d = 'lin3' /* three-dimensional line chart */,
	Keynote5KCctPie_2d = 'pie2' /* two-dimensional pie chart */,
	Keynote5KCctPie_3d = 'pie3' /* three-dimensional pie chart. */,
	Keynote5KCctScatterplot_2d = 'scp2' /* two-dimensional scatterplot chart */,
	Keynote5KCctStacked_area_2d = 'sar2' /* two-dimensional stacked area chart */,
	Keynote5KCctStacked_area_3d = 'sar3' /* three-dimensional stacked area chart */,
	Keynote5KCctStacked_horizontal_bar_2d = 'shb2' /* two-dimensional stacked horizontal bar chart */,
	Keynote5KCctStacked_horizontal_bar_3d = 'shb3' /* three-dimensional stacked horizontal bar chart */,
	Keynote5KCctStacked_vertical_bar_2d = 'svb2' /* two-dimensional stacked vertical bar chart */,
	Keynote5KCctStacked_vertical_bar_3d = 'svb3' /* three-dimensional stacked bar chart */,
	Keynote5KCctVertical_bar_2d = 'vbr2' /* two-dimensional vertical bar chart */,
	Keynote5KCctVertical_bar_3d = 'vbr3' /* three-dimensional vertical bar chart */
};
typedef enum Keynote5KCct Keynote5KCct;

enum Keynote5KCgb {
	Keynote5KCgbColumn = 'KCgc' /* group by column */,
	Keynote5KCgbRow = 'KCgr' /* group by row */
};
typedef enum Keynote5KCgb Keynote5KCgb;

enum Keynote5KCef {
	Keynote5KCefHTML = 'Khtm' /* HTML */,
	Keynote5KCefKPF_RAW = 'Kkpf' /* Raw KPF */
};
typedef enum Keynote5KCef Keynote5KCef;

enum Keynote5Enum {
	Keynote5EnumStandard = 'lwst' /* Standard PostScript error handling */,
	Keynote5EnumDetailed = 'lwdt' /* print a detailed report of PostScript errors */
};
typedef enum Keynote5Enum Keynote5Enum;



/*
 * Standard Suite
 */

// A scriptable object.
@interface Keynote5Item : SBObject

@property (copy) NSDictionary *properties;  // All of the object's properties.

- (void) closeSaving:(Keynote5Savo)saving savingIn:(NSURL *)savingIn;  // Close an object.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy object(s) and put the copies at a new location.
- (BOOL) exists;  // Verify if an object exists.
- (void) moveTo:(SBObject *)to;  // Move object(s) to a new location.
- (void) saveAs:(NSString *)as in:(NSURL *)in_;  // Save an object.
- (void) addChartColumnNames:(NSString *)columnNames data:(NSInteger)data groupBy:(Keynote5KCgb)groupBy rowNames:(NSString *)rowNames type:(Keynote5KCct)type;  // Add a chart to a slide
- (void) addFilePath:(NSString *)path;  // Add an image to a slide
- (void) advance;  // Advance one build or slide
- (void) makeImageSlidesPaths:(NSString *)paths master:(Keynote5MasterSlide *)master setTitles:(BOOL)setTitles;  // Make a series of slides from a list of image paths.  Returns a list of paths from which new slides could not be made.
- (void) pauseSlideshow;  // Pause the slideshow
- (void) resumeSlideshow;  // Resume the slideshow
- (void) showNext;  // Advance one build or slide
- (void) showPrevious;  // Go to the previous slide
- (void) start;  // Play an object.
- (void) startFrom;  // Play the containing slideshow starting with this object
- (void) stopSlideshow;  // Stop the slideshow

@end

// An application's top level scripting object.
@interface Keynote5Application : SBApplication

- (SBElementArray *) slideshows;
- (SBElementArray *) windows;

@property (readonly) BOOL frontmost;  // Is this the frontmost (active) application?
@property (copy, readonly) NSString *name;  // The name of the application.
@property (copy, readonly) NSString *version;  // The version of the application.

- (Keynote5Slideshow *) open:(NSURL *)x;  // Open an object.
- (void) print:(NSURL *)x printDialog:(BOOL)printDialog withProperties:(Keynote5PrintSettings *)withProperties;  // Print an object.
- (void) quitSaving:(Keynote5Savo)saving;  // Quit an application.
- (void) acceptSlideSwitcher;  // Hide the slide switcher, going to the slide it has selected
- (void) cancelSlideSwitcher;  // Hide the slide switcher without changing slides
- (void) GetURL:(NSString *)x;  // Open and start the document at the given URL.  Must be a file URL.
- (void) moveSlideSwitcherBackward;  // Move the slide switcher backward one slide
- (void) moveSlideSwitcherForward;  // Move the slide switcher forward one slide
- (void) pause;  // Pause the slideshow
- (void) showSlideSwitcher;  // Show the slide switcher in play mode

@end

// A color.
@interface Keynote5Color : Keynote5Item


@end

// A document.
@interface Keynote5Slideshow : Keynote5Item

@property (readonly) BOOL modified;  // Has the document been modified since the last save?
@property (copy) NSString *name;  // The document's name.
@property (copy) NSString *path;  // The document's path.

- (void) exportTo:(NSURL *)to as:(Keynote5KCef)as;  // Export a slideshow to another file

@end

// A window.
@interface Keynote5Window : Keynote5Item

@property NSRect bounds;  // The bounding rectangle of the window.
@property (readonly) BOOL closeable;  // Whether the window has a close box.
@property (copy, readonly) Keynote5Slideshow *document;  // The document whose contents are being displayed in the window.
@property (readonly) BOOL floating;  // Whether the window floats.
- (NSInteger) id;  // The unique identifier of the window.
@property NSInteger index;  // The index of the window, ordered front to back.
@property (readonly) BOOL miniaturizable;  // Whether the window can be miniaturized.
@property BOOL miniaturized;  // Whether the window is currently miniaturized.
@property (readonly) BOOL modal;  // Whether the window is the application's current modal window.
@property (copy) NSString *name;  // The full title of the window.
@property (readonly) BOOL resizable;  // Whether the window can be resized.
@property (readonly) BOOL titled;  // Whether the window has a title bar.
@property BOOL visible;  // Whether the window is currently visible.
@property (readonly) BOOL zoomable;  // Whether the window can be zoomed.
@property BOOL zoomed;  // Whether the window is currently zoomed.


@end



/*
 * Text Suite
 */

// This subdivides the text into chunks that all have the same attributes.
@interface Keynote5AttributeRun : Keynote5Item

- (SBElementArray *) attachments;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;

@property (copy) NSColor *color;  // The color of the first character.
@property (copy) NSString *font;  // The name of the font of the first character.
@property NSInteger size;  // The size in points of the first character.


@end

// This subdivides the text into characters.
@interface Keynote5Character : Keynote5Item

- (SBElementArray *) attachments;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;

@property (copy) NSColor *color;  // The color of the first character.
@property (copy) NSString *font;  // The name of the font of the first character.
@property NSInteger size;  // The size in points of the first character.


@end

// This subdivides the text into paragraphs.
@interface Keynote5Paragraph : Keynote5Item

- (SBElementArray *) attachments;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;

@property (copy) NSColor *color;  // The color of the first character.
@property (copy) NSString *font;  // The name of the font of the first character.
@property NSInteger size;  // The size in points of the first character.


@end

// Rich (styled) text
@interface Keynote5Text : Keynote5Item

- (SBElementArray *) attachments;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;

@property (copy) NSColor *color;  // The color of the first character.
@property (copy) NSString *font;  // The name of the font of the first character.
@property NSInteger size;  // The size in points of the first character.

- (void) GetURL;  // Open and start the document at the given URL.  Must be a file URL.

@end

// Represents an inline text attachment.  This class is used mainly for make commands.
@interface Keynote5Attachment : Keynote5Text

@property (copy) NSString *fileName;  // The path to the file for the attachment


@end

// This subdivides the text into words.
@interface Keynote5Word : Keynote5Item

- (SBElementArray *) attachments;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;

@property (copy) NSColor *color;  // The color of the first character.
@property (copy) NSString *font;  // The name of the font of the first character.
@property NSInteger size;  // The size in points of the first character.


@end



/*
 * Keynote5 Suite
 */

// Keynote5's top level scripting object.
@interface Keynote5Application (Keynote5Suite)

- (SBElementArray *) appThemes;
- (SBElementArray *) appTransitions;

@property BOOL frozen;  // Is Keynote5 frozen during playback?  When true, the show is playing but no motion occurs.
@property (readonly) BOOL playing;  // Is Keynote5 playing a show?
@property (readonly) BOOL slideSwitcherVisible;  // Is the slide selector visible?

@end

// The themes available to the appliction
@interface Keynote5AppTheme : Keynote5Item

- (NSInteger) id;  // The unique identifier of this slide.
@property (copy, readonly) NSString *name;  // The name of the theme, as it would appear in the theme chooser.


@end

// The transistions available for applying to a slide.
@interface Keynote5AppTransition : Keynote5Item

@property (copy, readonly) NSDictionary *attributes;  // Map of attribute names to potential values
@property (copy, readonly) NSString *name;  // The name of the transition.


@end

// A theme as applied to a document
@interface Keynote5DocTheme : Keynote5Item

- (SBElementArray *) masterSlides;


@end

// A master slide in a document's theme.
@interface Keynote5MasterSlide : Keynote5Item

- (SBElementArray *) slides;

- (NSInteger) id;  // The unique identifier of this slide.
@property (copy, readonly) NSString *name;  // The name of the master slide.


@end

// A slide in a slideshow
@interface Keynote5Slide : Keynote5Item

@property (copy) NSString *body;  // The body text of this slide.
- (NSInteger) id;  // The unique identifier of this slide.
@property (copy) Keynote5MasterSlide *master;  // The master of the slide.
@property (copy) NSString *notes;  // The speaker's notes for this slide.
@property BOOL skipped;  // Whether the slide is hidden.
@property (readonly) NSInteger slideNumber;  // index of the slide in the document
@property (copy) NSString *title;  // The title of this slide.
@property (copy, readonly) Keynote5SlideTransition *transition;  // The transition of the slide

- (void) jumpTo;  // Jump to the given slide
- (void) show;  // Show (or jump to) the recipient.

@end

// A slideshow
@interface Keynote5Slideshow (Keynote5Suite)

- (SBElementArray *) docThemes;
- (SBElementArray *) masterSlides;
- (SBElementArray *) slides;

@property (copy) Keynote5Slide *currentSlide;  // The slide that is currently selected.
@property (readonly) BOOL playing;  // Is Keynote5 playing the receiving document?

@end

// The transition of a slide
@interface Keynote5SlideTransition : Keynote5Item

@property (copy, readonly) NSDictionary *attributes;  // Map of attribute names to values
@property (copy) Keynote5AppTransition *type;  // The type of the transition


@end



/*
 * Type Definitions
 */

@interface Keynote5PrintSettings : SBObject

@property NSInteger copies;  // the number of copies of a document to be printed
@property BOOL collating;  // Should printed copies be collated?
@property NSInteger startingPage;  // the first page of the document to be printed
@property NSInteger endingPage;  // the last page of the document to be printed
@property NSInteger pagesAcross;  // number of logical pages laid across a physical page
@property NSInteger pagesDown;  // number of logical pages laid out down a physical page
@property (copy) NSDate *requestedPrintTime;  // the time at which the desktop printer should print the document
@property Keynote5Enum errorHandling;  // how errors are handled
@property (copy) NSString *faxNumber;  // for fax number
@property (copy) NSString *targetPrinter;  // for target printer

- (void) closeSaving:(Keynote5Savo)saving savingIn:(NSURL *)savingIn;  // Close an object.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy object(s) and put the copies at a new location.
- (BOOL) exists;  // Verify if an object exists.
- (void) moveTo:(SBObject *)to;  // Move object(s) to a new location.
- (void) saveAs:(NSString *)as in:(NSURL *)in_;  // Save an object.
- (void) addChartColumnNames:(NSString *)columnNames data:(NSInteger)data groupBy:(Keynote5KCgb)groupBy rowNames:(NSString *)rowNames type:(Keynote5KCct)type;  // Add a chart to a slide
- (void) addFilePath:(NSString *)path;  // Add an image to a slide
- (void) advance;  // Advance one build or slide
- (void) makeImageSlidesPaths:(NSString *)paths master:(Keynote5MasterSlide *)master setTitles:(BOOL)setTitles;  // Make a series of slides from a list of image paths.  Returns a list of paths from which new slides could not be made.
- (void) pauseSlideshow;  // Pause the slideshow
- (void) resumeSlideshow;  // Resume the slideshow
- (void) showNext;  // Advance one build or slide
- (void) showPrevious;  // Go to the previous slide
- (void) start;  // Play an object.
- (void) startFrom;  // Play the containing slideshow starting with this object
- (void) stopSlideshow;  // Stop the slideshow

@end


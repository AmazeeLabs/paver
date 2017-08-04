(function ($) {
  /**
   * Attach toggling behavior and notify the overlay of the toolbar.
   */
  Drupal.behaviors.paver = {
    attach: function(context) {
      $('.paver-image', context).paver();
    }
  };

})(jQuery);

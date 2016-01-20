{if $MessageCallback && is_object($MessageCallback) && $MessageCallback->validMessage()}
To download, go to {$MessageCallback->getMessageText()}

{else}
    Unexpected message
{/if}
To block, call 1-877-4MIXXER.

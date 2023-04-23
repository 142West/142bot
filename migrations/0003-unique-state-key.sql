ALTER TABLE bot_state DROP CONSTRAINT IF EXISTS setting_unique;
ALTER TABLE bot_state ADD CONSTRAINT setting_unique UNIQUE (setting);
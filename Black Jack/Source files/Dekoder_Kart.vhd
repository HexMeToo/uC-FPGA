library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Dekoder_Kart is
	port(	CLK: in std_logic;
			D: in std_logic_vector(3 downto 0);
			Q: buffer std_logic_vector(3 downto 0) );
end Dekoder_Kart;

architecture Behavioral of Dekoder_Kart is
begin
	process(clk)
	begin
		if clk'event and clk = '1' then
			case D is
				when "0000" => Q <="0010";
				when "0001" => Q <="0011";
				when "0010" => Q <="0100";
				when "0011" => Q <="0101";
				when "0100" => Q <="0110";
				when "0101" => Q <="0111";
				when "0110" => Q <="1000";
				when "0111" => Q <="1001";
				when "1000" => Q <="1010";
				when "1001" => Q <="0010";
				when "1010" => Q <="0011";
				when "1011" => Q <="0100";
				when "1100" => Q <="1011";
				when others => Q <="0000";
			end case;
		end if;
	end process;
end Behavioral;